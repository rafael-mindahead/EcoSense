import "./navigation.js";
import {
    getApiHealth,
    getLatestMeasurement,
    getMeasurements,
    getDevices,
    getAlerts,
    getStatus,
    controlFan,
    controlExhaust
} from "./api.js";

let currentDeviceId = 1;
let measurements = [];

function showText(id, value) {
    const element = document.getElementById(id);
    if (element) element.textContent = value;
}

function formatDate(value) {
    if (!value) return "--";
    const date = new Date(value);
    return Number.isNaN(date.getTime())
        ? value
        : date.toLocaleString("pt-BR", {
            dateStyle: "short",
            timeStyle: "short"
        });
}

async function loadApiStatus() {
    const status = document.querySelector(".system-status");
    const dot = document.querySelector(".status-dot");
    if (!status) return;

    try {
        await getApiHealth();
        status.querySelector("strong").textContent = "ESP32 Online";
        status.querySelector("small").textContent = "Servidor embarcado";
        if (dot) dot.style.background = "#46a56e";
    } catch {
        status.querySelector("strong").textContent = "ESP32 Offline";
        status.querySelector("small").textContent = "Sem resposta do dispositivo";
        if (dot) dot.style.background = "#b74b4b";
    }
}

async function loadLatestMeasurement() {
    const measurement = await getLatestMeasurement();
    showText("temperature", measurement.dht_valid
        ? Number(measurement.temperature).toFixed(1) + " °C"
        : "N/A");
    showText("humidity", measurement.dht_valid
        ? Number(measurement.humidity).toFixed(1) + " %"
        : "N/A");
    showText("luminosity", Number(measurement.luminosity || 0).toFixed(0) + " ADC");
    showText("air-quality", Number(measurement.air_quality || 0).toFixed(0));
    showText("last-update", formatDate(measurement.created_at));
}

async function loadDevices() {
    const devices = await getDevices();
    const device = devices[0];
    currentDeviceId = Number(device.id || 1);
    showText("device-name", device.name);
    showText("device-status", device.status);
    showText("device-code", "Código: " + device.device_code);
    showText("device-panel-name", device.name);
    showText("device-panel-status", "Status: " + device.status);
    showText("device-last-seen", "Última comunicação: " + formatDate(device.last_seen));
}

async function loadAlerts() {
    const alerts = await getAlerts();
    showText("active-alerts", alerts.length);
    showText("alert-count", alerts.length);

    const container = document.getElementById("alerts-list");
    if (!container) return;
    container.innerHTML = "";

    if (!alerts.length) {
        container.innerHTML = '<p class="empty-message">Nenhum warning ou erro persistido.</p>';
        return;
    }

    alerts.slice(-3).reverse().forEach(alert => {
        const item = document.createElement("div");
        item.className = "alert-item";
        const title = document.createElement("strong");
        title.textContent = alert.level;
        const body = document.createElement("p");
        body.textContent = alert.message || "Evento registrado";
        const date = document.createElement("small");
        date.textContent = alert.created_at || "--";
        item.append(title, body, date);
        container.appendChild(item);
    });
}

function drawChart(metric) {
    const canvas = document.getElementById("sensor-chart");
    if (!canvas) return;

    const ratio = window.devicePixelRatio || 1;
    const width = Math.max(canvas.clientWidth, 320);
    const height = Math.max(canvas.clientHeight, 260);
    canvas.width = width * ratio;
    canvas.height = height * ratio;

    const ctx = canvas.getContext("2d");
    ctx.scale(ratio, ratio);
    ctx.clearRect(0, 0, width, height);

    const values = measurements
        .map(item => Number(item[metric]))
        .filter(Number.isFinite);

    if (!values.length) {
        ctx.fillStyle = "#68736d";
        ctx.font = "14px sans-serif";
        ctx.fillText("Sem histórico disponível", 20, 30);
        return;
    }

    const padding = 24;
    const min = Math.min(...values);
    const max = Math.max(...values);
    const range = Math.max(max - min, 1);

    ctx.strokeStyle = "#2f6f4f";
    ctx.lineWidth = 2;
    ctx.beginPath();

    values.forEach((value, index) => {
        const x = padding + index * ((width - padding * 2) / Math.max(values.length - 1, 1));
        const y = height - padding - ((value - min) / range) * (height - padding * 2);
        if (index === 0) ctx.moveTo(x, y);
        else ctx.lineTo(x, y);
    });

    ctx.stroke();
}

async function loadDashboardChart() {
    measurements = await getMeasurements();
    drawChart(document.getElementById("chart-filter")?.value || "temperature");
}

async function sendActuatorCommand(actuator, command, button) {
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
        alert("Não foi possível enviar o comando: " + error.message);
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
        loadDashboardChart(),
        getStatus()
    ]);
}

document.addEventListener("DOMContentLoaded", async () => {
    setupActuatorButtons();
    document.getElementById("chart-filter")?.addEventListener("change", event =>
        drawChart(event.target.value)
    );
    await refreshDashboard();
    setInterval(refreshDashboard, 5000);
});
