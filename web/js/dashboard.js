import {
    getApiHealth,
    getLatestMeasurement,
    getMeasurements,
    getDevices,
    getAlerts,
    controlFan,
    controlExhaust
} from "./api.js";

let currentDeviceId = null;

function formatDate(dateString) {
    if (!dateString) {
        return "--";
    }

    const date = new Date(dateString);

    if (Number.isNaN(date.getTime())) {
        return dateString;
    }

    return date.toLocaleString(
        "pt-BR",
        {
            dateStyle: "short",
            timeStyle: "short"
        }
    );
}


function showText(
    elementId,
    value
) {
    const element =
        document.getElementById(elementId);

    if (element) {
        element.textContent = value;
    }
}

async function loadApiStatus() {
    const systemStatus =
        document.querySelector(
            ".system-status"
        );

    const statusDot =
        document.querySelector(
            ".status-dot"
        );

    if (!systemStatus) {
        return;
    }

    try {
        const response =
            await getApiHealth();

        const title =
            systemStatus.querySelector(
                "strong"
            );

        const description =
            systemStatus.querySelector(
                "small"
            );

        if (title) {
            title.textContent =
                "API Online";
        }

        if (description) {
            description.textContent =
                response.service ||
                "Sistema operacional";
        }

        if (statusDot) {
            statusDot.style.background =
                "#46a56e";
        }

    } catch (error) {

        const title =
            systemStatus.querySelector(
                "strong"
            );

        const description =
            systemStatus.querySelector(
                "small"
            );

        if (title) {
            title.textContent =
                "API Offline";
        }

        if (description) {
            description.textContent =
                "Não foi possível conectar";
        }

        if (statusDot) {
            statusDot.style.background =
                "#b74b4b";
        }

        console.error(
            "Erro ao verificar API:",
            error
        );
    }
}

async function loadLatestMeasurement() {
     try {
        const measurement = await getLatestMeasurement();

        if (!measurement || typeof measurement !== "object") {
            throw new Error("Medição inválida");
        }

        showText(
            "temperature",
            formatSensor(measurement.temperature, "°C", 1)
        );

        showText(
            "humidity",
            formatSensor(measurement.humidity, "%", 1)
        );

        showText(
            "luminosity",
            formatSensor(measurement.luminosity, "lx", 0)
        );

        showText(
            "air-quality",
            formatSensor(measurement.air_quality, "", 0)
        );

        showText(
            "last-update",
            formatDate(measurement.created_at)
        );

        if (!currentDeviceId && measurement.device_id) {
            currentDeviceId = Number(measurement.device_id);
        }

    } catch (error) {
        console.error("Erro ao carregar medições:", error);

        showText("temperature", "-- °C");
        showText("humidity", "-- %");
        showText("luminosity", "-- lx");
        showText("air-quality", "--");

        showText(
            "last-update",
            "Medições indisponíveis"
        );
    }
}

function formatSensor(value, unit, decimals) {
    if (value === null || value === undefined || value === "") {
        return "--" + (unit ? ` ${unit}` : "");
    }

    const number = Number(value);

    if (!Number.isFinite(number)) {
        return "--" + (unit ? ` ${unit}` : "");
    }

    const formatted = number.toFixed(decimals);

    return unit ? `${formatted} ${unit}` : formatted;
}

async function loadDevices() {
    try {
        const response =
            await getDevices();

        const devices =
            Array.isArray(response)
                ? response
                : response.data || [];

        if (devices.length === 0) {

            showText(
                "device-name",
                "Nenhum dispositivo"
            );

            showText(
                "device-status",
                "Indisponível"
            );

            return;
        }

        const device =
            devices[0];

        currentDeviceId =
            Number(device.id);

        showText(
            "device-name",
            device.name ||
            "Dispositivo"
        );

        showText(
            "device-status",
            device.status ||
            "--"
        );

        showText(
            "device-code",
            `Código: ${
                device.device_code ||
                "--"
            }`
        );

        showText(
            "device-panel-name",
            device.name ||
            "ESP32"
        );

        showText(
            "device-panel-status",
            `Status: ${
                device.status ||
                "--"
            }`
        );

        showText(
            "device-last-seen",
            `Última comunicação: ${
                formatDate(
                    device.last_seen
                )
            }`
        );

        updateDeviceStatusStyle(
            device.status
        );

    } catch (error) {

        console.error(
            "Erro ao carregar dispositivos:",
            error
        );

        showText(
            "device-status",
            "Erro"
        );
    }
}

function updateDeviceStatusStyle(
    status
) {
    const element =
        document.getElementById(
            "device-status"
        );

    if (!element) {
        return;
    }

    const normalizedStatus =
        String(status || "")
            .toLowerCase();

    if (
        normalizedStatus ===
        "online"
    ) {
        element.style.color =
            "#2f6f4f";
    } else {
        element.style.color =
            "#b74b4b";
    }
}

async function loadAlerts() {
    try {
        const response =
            await getAlerts();

        const alerts =
            response.data || [];

        const openAlerts =
            alerts.filter(
                alert =>
                    alert.status ===
                    "open"
            );

        showText(
            "active-alerts",
            openAlerts.length
        );

        showText(
            "alert-count",
            openAlerts.length
        );

        renderRecentAlerts(
            openAlerts
        );

    } catch (error) {

        console.error(
            "Erro ao carregar alertas:",
            error
        );
    }
}

function renderRecentAlerts(
    alerts
) {
    const container =
        document.getElementById(
            "alerts-list"
        );

    if (!container) {
        return;
    }

    container.innerHTML = "";

    if (alerts.length === 0) {

        const paragraph =
            document.createElement(
                "p"
            );

        paragraph.className =
            "empty-message";

        paragraph.textContent =
            "Nenhum alerta ativo.";

        container.appendChild(
            paragraph
        );

        return;
    }

    const recentAlerts =
        alerts.slice(0, 3);

    recentAlerts.forEach(
        alert => {

            const item =
                document.createElement(
                    "div"
                );

            item.className =
                "alert-item";

            const title =
                document.createElement(
                    "strong"
                );

            title.textContent =
                getMetricLabel(
                    alert.metric
                );

            const message =
                document.createElement(
                    "p"
                );

            message.textContent =
                alert.message ||
                "Alerta detectado";

            const date =
                document.createElement(
                    "small"
                );

            date.textContent =
                formatDate(
                    alert.created_at
                );

            item.appendChild(
                title
            );

            item.appendChild(
                message
            );

            item.appendChild(
                date
            );

            container.appendChild(
                item
            );
        }
    );
}

function getMetricLabel(metric) {
    const labels = {
        temperature:
            "Temperatura",

        humidity:
            "Umidade",

        luminosity:
            "Luminosidade",

        air_quality:
            "Qualidade do ar"
    };

    return (
        labels[metric] ||
        metric ||
        "Sensor"
    );
}

async function sendActuatorCommand(
    actuator,
    command
) {
    if (!currentDeviceId) {

        alert(
            "Nenhum dispositivo disponível."
        );

        return;
    }

    try {

        if (
            actuator === "fan"
        ) {
            await controlFan(
                currentDeviceId,
                command
            );

            showText(
                "fan-status",
                command === "ON"
                    ? "Ligado"
                    : "Desligado"
            );
        }

        if (
            actuator ===
            "exhaust"
        ) {
            await controlExhaust(
                currentDeviceId,
                command
            );

            showText(
                "exhaust-status",
                command === "ON"
                    ? "Ligado"
                    : "Desligado"
            );
        }

    } catch (error) {

        console.error(
            "Erro ao controlar atuador:",
            error
        );

        alert(
            "Não foi possível enviar o comando."
        );
    }
}

function setupActuatorButtons() {

    const fanOn =
        document.getElementById(
            "fan-on"
        );

    const fanOff =
        document.getElementById(
            "fan-off"
        );

    const exhaustOn =
        document.getElementById(
            "exhaust-on"
        );

    const exhaustOff =
        document.getElementById(
            "exhaust-off"
        );


    fanOn?.addEventListener(
        "click",
        () => {
            sendActuatorCommand(
                "fan",
                "ON"
            );
        }
    );


    fanOff?.addEventListener(
        "click",
        () => {
            sendActuatorCommand(
                "fan",
                "OFF"
            );
        }
    );


    exhaustOn?.addEventListener(
        "click",
        () => {
            sendActuatorCommand(
                "exhaust",
                "ON"
            );
        }
    );


    exhaustOff?.addEventListener(
        "click",
        () => {
            sendActuatorCommand(
                "exhaust",
                "OFF"
            );
        }
    );
}

let sensorChart = null;

async function loadSensorChart() {
    const canvas = document.getElementById("sensor-chart");
    const filter = document.getElementById("chart-filter");

    if (!canvas || !filter || typeof Chart === "undefined") {
        return;
    }

    try {
        const response = await getMeasurements(50);

        const measurements = Array.isArray(response)
            ? response
            : response.data || [];

        // A API retorna os registros mais recentes primeiro.
        // Invertemos para exibir do mais antigo ao mais novo.
        const ordered = [...measurements].reverse();

        const metrics = {
            temperature: {
                label: "Temperatura",
                unit: "°C",
                color: "#e88a62"
            },
            humidity: {
                label: "Umidade",
                unit: "%",
                color: "#68afd4"
            },
            luminosity: {
                label: "Luminosidade",
                unit: "lx",
                color: "#dfb443"
            },
            air_quality: {
                label: "Qualidade do ar",
                unit: "",
                color: "#368c5b"
            }
        };

        const metric = filter.value;
        const selected = metrics[metric] || metrics.temperature;

        const labels = ordered.map(measurement =>
            formatDate(measurement.created_at)
        );

        const values = ordered.map(measurement =>
            measurement[metric] == null
                ? null
                : Number(measurement[metric])
        );

        if (sensorChart) {
            sensorChart.destroy();
        }

        sensorChart = new Chart(canvas, {
            type: "line",

            data: {
                labels: labels,
                datasets: [{
                    label: selected.label,
                    data: values,
                    borderColor: selected.color,
                    backgroundColor: selected.color,
                    borderWidth: 3,
                    tension: 0.3,
                    pointRadius: 5,
                    pointHoverRadius: 7,
                    spanGaps: false
                }]
            },

            options: {
                responsive: true,
                maintainAspectRatio: false,

                plugins: {
                    legend: {
                        display: true,
                        position: "top"
                    },
                    tooltip: {
                        callbacks: {
                            label: context =>
                                `${selected.label}: ${context.parsed.y} ${selected.unit}`
                        }
                    }
                },

                scales: {
                    x: {
                        ticks: {
                            maxTicksLimit: 6
                        }
                    },
                    y: {
                        beginAtZero: false
                    }
                }
            }
        });

    } catch (error) {
        console.error("Erro ao carregar gráfico:", error);
    }
}

async function refreshDashboard() {
    await Promise.allSettled([
        loadApiStatus(),
        loadLatestMeasurement(),
        loadDevices(),
        loadAlerts(),
        loadSensorChart()
    ]);
}

document.addEventListener("DOMContentLoaded", async () => {

    setupActuatorButtons();

    const filter = document.getElementById("chart-filter");

    filter?.addEventListener("change", loadSensorChart);

    await refreshDashboard();

    setInterval(refreshDashboard, 5000);
});