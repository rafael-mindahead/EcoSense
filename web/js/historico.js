import { getMeasurements, getDevices } from "./api.js";

// =====================================
// VARIÁVEIS GLOBAIS
// =====================================

const charts = {};

let measurements = [];
let devices = [];

// =====================================
// FORMATAÇÃO DOS DADOS
// =====================================

// Formata datas no padrão brasileiro
const fmt = (value) => {
    if (!value) {
        return "--";
    }

    const date = new Date(value);

    return Number.isNaN(date.getTime())
        ? value
        : date.toLocaleString("pt-BR", {
            dateStyle: "short",
            timeStyle: "short"
        });
};

// Formata valores numéricos
const num = (value, decimals = 1) => {
    if (value === null || value === undefined || value === "") {
        return "--";
    }

    return Number.isFinite(Number(value))
        ? Number(value).toFixed(decimals)
        : "--";
};

// Busca o nome do dispositivo pelo ID
const devName = (id) => {
    const device = devices.find(
        (item) => Number(item.id) === Number(id)
    );

    return device?.name || `Dispositivo ${id ?? "--"}`;
};

// =====================================
// FILTRAGEM E ORDENAÇÃO
// =====================================

// Filtra as medições pelo dispositivo selecionado
function rows() {
    const selected = document.getElementById("history-device").value;

    return measurements
        .filter((item) => {
            return selected === "all" ||
                Number(item.device_id) === Number(selected);
        })
        .slice()
        .sort((a, b) => {
            return new Date(a.created_at) - new Date(b.created_at);
        });
}

// =====================================
// DATAS DOS GRÁFICOS
// =====================================

// Cria os rótulos do eixo horizontal
function labels(rows) {
    return rows.map((item) => {
        const date = new Date(item.created_at);

        if (Number.isNaN(date.getTime())) {
            return String(item.created_at ?? "--");
        }

        return date.toLocaleString("pt-BR", {
            day: "2-digit",
            month: "2-digit",
            hour: "2-digit",
            minute: "2-digit"
        });
    });
}

// =====================================
// CRIAÇÃO DOS GRÁFICOS
// =====================================

// Destrói o gráfico anterior e cria um novo
function make(name, elementId, type, data, options = {}) {
    if (charts[name]) {
        charts[name].destroy();
    }

    charts[name] = new Chart(
        document.getElementById(elementId),
        {
            type: type,
            data: data,

            options: {
                responsive: true,
                maintainAspectRatio: false,

                interaction: {
                    mode: "index",
                    intersect: false
                },

                plugins: {
                    legend: {
                        position: "bottom"
                    }
                },

                ...options
            }
        }
    );
}

// =====================================
// EXIBIÇÃO DOS GRÁFICOS
// =====================================

function renderCharts(rows) {
    const chartLabels = labels(rows);

    // GRÁFICO 1 - TEMPERATURA E UMIDADE
    make(
        "environment",
        "environment-chart",
        "line",
        {
            labels: chartLabels,

            datasets: [
                {
                    label: "Temperatura (°C)",
                    data: rows.map(
                        (item) => Number(item.temperature)
                    ),
                    borderWidth: 2,
                    tension: 0.25,
                    pointRadius: 2
                },
                {
                    label: "Umidade (%)",
                    data: rows.map(
                        (item) => Number(item.humidity)
                    ),
                    borderWidth: 2,
                    tension: 0.25,
                    pointRadius: 2
                }
            ]
        }
    );

    // GRÁFICO 2 - LUMINOSIDADE
    make(
        "luminosity",
        "luminosity-chart",
        "line",
        {
            labels: chartLabels,

            datasets: [
                {
                    label: "Luminosidade (lx)",
                    data: rows.map(
                        (item) => Number(item.luminosity)
                    ),
                    borderWidth: 2,
                    tension: 0.25,
                    pointRadius: 2
                }
            ]
        },
        {
            scales: {
                y: {
                    beginAtZero: true
                }
            }
        }
    );

    // GRÁFICO 3 - QUALIDADE DO AR
    make(
        "air",
        "air-chart",
        "line",
        {
            labels: chartLabels,

            datasets: [
                {
                    label: "Qualidade do ar",
                    data: rows.map(
                        (item) => Number(item.air_quality)
                    ),
                    borderWidth: 2,
                    tension: 0.25,
                    pointRadius: 2
                }
            ]
        },
        {
            scales: {
                y: {
                    beginAtZero: true
                }
            }
        }
    );
}

// =====================================
// EXIBIÇÃO DA TABELA
// =====================================

function renderTable(rows) {
    const message = document.getElementById("history-message");

    const wrapper = document.getElementById(
        "history-table-wrapper"
    );

    const body = document.getElementById(
        "history-table-body"
    );

    body.innerHTML = "";

    // Nenhuma medição encontrada
    if (!rows.length) {
        wrapper.hidden = true;
        message.hidden = false;

        message.textContent =
            "Nenhuma medição encontrada para o filtro selecionado.";

        document.getElementById("history-summary").textContent =
            "0 medições";

        return;
    }

    // Mostra a tabela
    message.hidden = true;
    wrapper.hidden = false;

    document.getElementById("history-summary").textContent =
        `${rows.length} medição(ões) carregada(s)`;

    // Exibe as medições mais recentes primeiro
    rows.slice().reverse().forEach((item) => {
        const tr = document.createElement("tr");

        const values = [
            fmt(item.created_at),
            devName(item.device_id),
            `${num(item.temperature)} °C`,
            `${num(item.humidity)} %`,
            `${num(item.luminosity, 0)} lx`,
            num(item.air_quality, 0)
        ];

        values.forEach((value) => {
            const td = document.createElement("td");
            td.textContent = value;
            tr.appendChild(td);
        });

        body.appendChild(tr);
    });
}

// =====================================
// ATUALIZAÇÃO DA PÁGINA
// =====================================

// Atualiza gráficos e tabela
function render() {
    const filteredRows = rows();

    renderCharts(filteredRows);
    renderTable(filteredRows);
}

// =====================================
// FILTRO DE DISPOSITIVOS
// =====================================

// Preenche o campo com os dispositivos cadastrados
function populate() {
    const select = document.getElementById("history-device");

    select.innerHTML = '<option value="all">Todos</option>';

    devices.forEach((device) => {
        const option = document.createElement("option");

        option.value = device.id;

        option.textContent =
            device.name || `Dispositivo ${device.id}`;

        select.appendChild(option);
    });
}

// =====================================
// CARREGAMENTO DOS DADOS DA API
// =====================================

async function load() {
    const message = document.getElementById("history-message");

    message.hidden = false;
    message.classList.remove("error-message");

    message.textContent =
        "Carregando as últimas 100 medições...";

    try {
        // Busca medições e dispositivos simultaneamente
        const [measurementsResponse, devicesResponse] =
            await Promise.all([
                getMeasurements(100),
                getDevices()
            ]);

        // Armazena as medições
        measurements = Array.isArray(measurementsResponse)
            ? measurementsResponse
            : measurementsResponse.data || [];

        // Armazena os dispositivos
        devices = Array.isArray(devicesResponse)
            ? devicesResponse
            : devicesResponse.data || [];

        // Preenche o filtro e atualiza a página
        populate();
        render();

    } catch (error) {
        console.error("Erro ao carregar histórico:", error);

        message.hidden = false;
        message.classList.add("error-message");

        message.textContent =
            `Não foi possível carregar o histórico: ${error.message}`;

        document.getElementById(
            "history-table-wrapper"
        ).hidden = true;
    }
}

// =====================================
// INICIALIZAÇÃO
// =====================================

document.addEventListener("DOMContentLoaded", () => {
    // Atualiza gráficos e tabela ao trocar o dispositivo
    document.getElementById("history-device")
        .addEventListener("change", render);

    // Busca novamente os dados ao clicar em Atualizar
    document.getElementById("history-refresh")
        .addEventListener("click", load);

    // Carrega os dados ao abrir a página
    load();
});
