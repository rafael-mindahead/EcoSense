import { getDevices } from "./api.js";
const $ = (id) => document.getElementById(id);
const metrics = {
    temperature: {
        label: "Temperatura",
        unit: "°C",
        color: "#b74b4b"
    },
    humidity: {
        label: "Umidade",
        unit: "%",
        color: "#3f825f"
    },
    luminosity: {
        label: "Luminosidade",
        unit: "lx",
        color: "#c98d2d"
    },
    air_quality: {
        label: "Qualidade do ar",
        unit: "",
        color: "#2f6f4f"
    }
};
let measurements = [];
let chart = null;
function dateText(value) {
    if (!value) return "--";
    const date = new Date(value);
    return Number.isNaN(date.getTime())
        ? value
        : date.toLocaleString("pt-BR");
}
function valueText(value, unit = "") {
    if (value == null) return "--";
    const number = Number(value);
    if (!Number.isFinite(number)) return "--";
    return `${number.toLocaleString("pt-BR", {
        maximumFractionDigits: 2
    })}${unit ? ` ${unit}` : ""}`;
}
function displayMessage(message) {
    $("history-message").textContent = message;
}
async function loadDevices() {
    try {
        const response = await getDevices();
        const devices = Array.isArray(response)
            ? response
            : (response.data ?? response.devices ?? []);
        for (const device of devices) {
            const option = document.createElement("option");
            option.value = device.id;
            option.textContent =
                device.name || `ESP32 #${device.id}`;
            $("device-filter").append(option);
        }
    } catch (error) {
        displayMessage(
            "Não foi possível carregar a lista de dispositivos."
        );
    }
}
async function loadHistory() {
    const from = $("date-from").value;
    const to = $("date-to").value;
    if (from && to && from > to) {
        displayMessage(
            "A data inicial não pode ser posterior à data final."
        );
        return;
    }
    const params = new URLSearchParams({
        limit: $("limit-filter").value
    });
    if ($("device-filter").value) {
        params.set("device_id", $("device-filter").value);
    }
    if (from) params.set("from", from);
    if (to) params.set("to", to);
    displayMessage("Carregando medições...");
    try {
        const response = await fetch(
            `/api/v1/meansurements?${params}`
        );
        const body = await response.json();
        if (!response.ok) {
            throw new Error(
                body.error ||
                body.message ||
                `Erro HTTP ${response.status}`
            );
        }
        measurements = Array.isArray(body)
            ? body
            : (body.data ?? []);

        if (!Array.isArray(measurements)) {
            throw new Error(
                "Formato inesperado no histórico da API."
            );
        }
        $("api-status").textContent = "API Online";
        document.querySelector(".status-dot")
            .style.background = "#46a56e";
        displayMessage(
            measurements.length
                ? ""
                : "Nenhuma medição encontrada para esses filtros."
        );
        renderTable();
        renderChart();
    } catch (error) {
        measurements = [];
        $("api-status").textContent = "API indisponível";
        document.querySelector(".status-dot")
            .style.background = "#b74b4b";
        displayMessage(
            `Não foi possível carregar o histórico: ${error.message}`
        );
        renderTable();
        renderChart();
    }
}
function renderTable() {
    $("history-summary").textContent =
        `${measurements.length} medição(ões) encontrada(s)`;
    const tbody = $("history-rows");
    tbody.replaceChildren();
    for (const m of measurements) {
        const tr = document.createElement("tr");
        const columns = [
            dateText(m.created_at),
            `#${m.device_id}`,
            valueText(m.temperature, "°C"),
            valueText(m.humidity, "%"),
            valueText(m.luminosity, "lx"),
            valueText(m.air_quality)
        ];
        for (const value of columns) {
            const td = document.createElement("td");
            td.textContent = value;
            tr.append(td);
        }
        tbody.append(tr);
    }
}
function renderChart() {
    if (chart) {
        chart.destroy();
        chart = null;
    }
    if (typeof Chart === "undefined") {
        displayMessage(
            "Chart.js não carregou. Confira sua conexão."
        );
        return;
    }
    const metric = $("metric-filter").value;
    const config = metrics[metric];
    const ordered = [...measurements].reverse();
    chart = new Chart($("history-chart"), {
        type: "line",
        data: {
            labels: ordered.map(
                m => dateText(m.created_at)
            ),
            datasets: [{
                label: `${config.label}${config.unit
                    ? ` (${config.unit})`
                    : ""}`,
                data: ordered.map(m => m[metric]),
                borderColor: config.color,
                backgroundColor: "rgba(63,130,95,.10)",
                tension: 0.25,
                pointRadius: 3,
                fill: true,
                spanGaps: false
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: {
                mode: "index",
                intersect: false
            },
            plugins: {
                legend: {
                    display: true
                }
            },
            scales: {
                x: {
                    ticks: {
                        maxTicksLimit: 7
                    }
                },
                y: {
                    beginAtZero: false
                }
            }
        }
    });
}
$("history-form").addEventListener("submit", (event) => {
    event.preventDefault();
    loadHistory();
});
$("metric-filter").addEventListener(
    "change",
    renderChart
);
loadDevices();
loadHistory();