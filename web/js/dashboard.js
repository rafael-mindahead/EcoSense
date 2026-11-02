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
let dashboardChart = null;
let measurements = [];

function formatDate(value) {
    if (!value) return "--";
    const date = new Date(value);
    return Number.isNaN(date.getTime())
        ? value
        : date.toLocaleString("pt-BR", { dateStyle: "short", timeStyle: "short" });
}

function showText(id, value) {
    const element = document.getElementById(id);
    if (element) element.textContent = value;
}

async function loadApiStatus() {
    const status = document.querySelector(".system-status");
    const dot = document.querySelector(".status-dot");
    if (!status) return;
    try {
        const response = await getApiHealth();
        status.querySelector("strong").textContent = "API Online";
        status.querySelector("small").textContent = response.service || "Sistema operacional";
        if (dot) dot.style.background = "#46a56e";
    } catch (error) {
        status.querySelector("strong").textContent = "API Offline";
        status.querySelector("small").textContent = "Não foi possível conectar";
        if (dot) dot.style.background = "#b74b4b";
    }
}

async function loadLatestMeasurement() {
    try {
        const measurement = await getLatestMeasurement();
        showText("temperature", `${Number(measurement.temperature).toFixed(1)} °C`);
        showText("humidity", `${Number(measurement.humidity).toFixed(1)} %`);
        showText("luminosity", `${Number(measurement.luminosity).toFixed(0)} lx`);
        showText("air-quality", `${Number(measurement.air_quality).toFixed(0)}`);
        showText("last-update", formatDate(measurement.created_at));
        if (!currentDeviceId) currentDeviceId = Number(measurement.device_id);
    } catch (error) {
        showText("last-update", "Não foi possível carregar os dados.");
    }
}

async function loadDevices() {
    try {
        const response = await getDevices();
        const devices = Array.isArray(response) ? response : response.data || [];
        if (!devices.length) {
            showText("device-name", "Nenhum dispositivo");
            showText("device-status", "Indisponível");
            return;
        }
        const device = devices[0];
        currentDeviceId = Number(device.id);
        showText("device-name", device.name || "Dispositivo");
        showText("device-status", device.status || "--");
        showText("device-code", `Código: ${device.device_code || "--"}`);
        showText("device-panel-name", device.name || "ESP32");
        showText("device-panel-status", `Status: ${device.status || "--"}`);
        showText("device-last-seen", `Última comunicação: ${formatDate(device.last_seen)}`);
        updateDeviceStatusStyle(device.status);
    } catch (error) {
        showText("device-status", "Erro");
    }
}

function updateDeviceStatusStyle(status) {
    const element = document.getElementById("device-status");
    if (!element) return;
    element.style.color = String(status || "").toLowerCase() === "online"
        ? "#2f6f4f"
        : "#b74b4b";
}

async function loadAlerts() {
    try {
        const response = await getAlerts();
        const alerts = response.data || [];
        const openAlerts = alerts.filter(alert => alert.status === "open");
        showText("active-alerts", openAlerts.length);
        showText("alert-count", openAlerts.length);
        renderRecentAlerts(openAlerts);
    } catch (error) {
        console.error("Erro ao carregar alertas:", error);
    }
}

function renderRecentAlerts(alerts) {
    const container = document.getElementById("alerts-list");
    if (!container) return;
    container.innerHTML = "";
    if (!alerts.length) {
        const p = document.createElement("p");
        p.className = "empty-message";
        p.textContent = "Nenhum alerta ativo.";
        container.appendChild(p);
        return;
    }
    alerts.slice(0, 3).forEach(alert => {
        const item = document.createElement("div");
        item.className = "alert-item";
        const title = document.createElement("strong");
        title.textContent = getMetricLabel(alert.metric);
        const message = document.createElement("p");
        message.textContent = alert.message || "Alerta detectado";
        const date = document.createElement("small");
        date.textContent = formatDate(alert.created_at);
        item.append(title, message, date);
        container.appendChild(item);
    });
}

function getMetricLabel(metric) {
    return {
        temperature: "Temperatura",
        humidity: "Umidade",
        luminosity: "Luminosidade",
        air_quality: "Qualidade do ar"
    }[metric] || metric || "Sensor";
}

async function loadDashboardChart() {
    try {
        const response = await getMeasurements(100);
        measurements = Array.isArray(response) ? response : response.data || [];
        renderDashboardChart(document.getElementById("chart-filter")?.value || "temperature");
    } catch (error) {
        console.error("Erro ao carregar histórico do gráfico:", error);
    }
}

function renderDashboardChart(metric) {
    const canvas = document.getElementById("sensor-chart");
    if (!canvas || typeof Chart === "undefined") return;

    const labels = measurements
        .slice()
        .sort((a, b) => new Date(a.created_at) - new Date(b.created_at))
        .map(item => formatDate(item.created_at));

    const dataset = measurements
        .slice()
        .sort((a, b) => new Date(a.created_at) - new Date(b.created_at))
        .map(item => Number(item[metric]));

    dashboardChart?.destroy();
    dashboardChart = new Chart(canvas, {
        type: "line",
        data: {
            labels,
            datasets: [{
                label: getMetricLabel(metric),
                data: dataset,
                borderWidth: 2,
                tension: .25,
                pointRadius: 2
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            interaction: { mode: "index", intersect: false },
            plugins: { legend: { position: "bottom" } },
            scales: {
                y: {
                    beginAtZero: metric === "luminosity" || metric === "air_quality"
                }
            }
        }
    });
}

async function sendActuatorCommand(actuator, command, button) {
    if (!currentDeviceId) {
        alert("Nenhum dispositivo disponível.");
        return;
    }

    const buttons = actuator === "fan"
        ? [document.getElementById("fan-on"), document.getElementById("fan-off")]
        : [document.getElementById("exhaust-on"), document.getElementById("exhaust-off")];

    buttons.forEach(item => { if (item) item.disabled = true; });
    const oldText = button?.textContent;
    if (button) button.textContent = "Enviando...";

    try {
        if (actuator === "fan") {
            await controlFan(currentDeviceId, command);
            showText("fan-status", command === "ON" ? "Ligado" : "Desligado");
        } else {
            await controlExhaust(currentDeviceId, command);
            showText("exhaust-status", command === "ON" ? "Ligado" : "Desligado");
        }
    } catch (error) {
        alert(`Não foi possível enviar o comando: ${error.message}`);
    } finally {
        buttons.forEach(item => { if (item) item.disabled = false; });
        if (button) button.textContent = oldText;
    }
}

function setupActuatorButtons() {
    [
        ["fan-on", "fan", "ON"],
        ["fan-off", "fan", "OFF"],
        ["exhaust-on", "exhaust", "ON"],
        ["exhaust-off", "exhaust", "OFF"]
    ].forEach(([id, actuator, command]) => {
        document.getElementById(id)?.addEventListener("click", event =>
            sendActuatorCommand(actuator, command, event.currentTarget)
        );
    });
}

async function refreshDashboard() {
    await Promise.allSettled([
        loadApiStatus(),
        loadLatestMeasurement(),
        loadDevices(),
        loadAlerts(),
        loadDashboardChart()
    ]);
}

document.addEventListener("DOMContentLoaded", async () => {
    setupActuatorButtons();
    document.getElementById("chart-filter")?.addEventListener("change", event =>
        renderDashboardChart(event.target.value)
    );
    await refreshDashboard();
    setInterval(refreshDashboard, 5000);
});