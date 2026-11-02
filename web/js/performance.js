import "./navigation.js";
import {
    getPerformance,
    getLatestMeasurement,
    getMeasurements,
    getStatus
} from "./api.js";

const text = (id, value) => {
    const element = document.getElementById(id);
    if (element) element.textContent = value;
};

function renderRows(id, rows, columns) {
    const body = document.getElementById(id);
    body.innerHTML = "";
    rows.forEach(row => {
        const tr = document.createElement("tr");
        columns.forEach(column => {
            const td = document.createElement("td");
            td.textContent = row[column] ?? "--";
            tr.appendChild(td);
        });
        body.appendChild(tr);
    });
}

function drawHistory(rows) {
    const canvas = document.getElementById("performance-chart");
    if (!canvas) return;
    const ratio = window.devicePixelRatio || 1;
    const width = Math.max(canvas.clientWidth, 320);
    const height = Math.max(canvas.clientHeight, 240);
    canvas.width = width * ratio;
    canvas.height = height * ratio;

    const ctx = canvas.getContext("2d");
    ctx.scale(ratio, ratio);
    ctx.clearRect(0, 0, width, height);

    const values = rows
        .filter(item => item.dht_valid)
        .map(item => Number(item.temperature))
        .filter(Number.isFinite);

    if (!values.length) {
        ctx.fillStyle = "#68736d";
        ctx.font = "14px sans-serif";
        ctx.fillText("Sem histórico de temperatura", 20, 30);
        return;
    }

    const min = Math.min(...values);
    const max = Math.max(...values);
    const range = Math.max(max - min, 1);
    const padding = 24;

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

async function load() {
    try {
        const [performance, sample, history, status] = await Promise.all([
            getPerformance(),
            getLatestMeasurement(),
            getMeasurements(),
            getStatus()
        ]);

        text("perf-status", "ESP32 Online");
        text("cpu", Number(performance.cpu_usage_percent_estimated || 0).toFixed(1));
        text("heap", performance.heap_free);
        text("flash", performance.flash_bytes);
        text("task-count", performance.task_count);

        document.getElementById("memory-network").textContent = JSON.stringify({
            cpu_frequency_mhz: performance.cpu_frequency_mhz,
            heap_total: performance.heap_total,
            heap_free: performance.heap_free,
            heap_min_free: performance.heap_min_free,
            psram_available: performance.psram_available,
            psram_total: performance.psram_total,
            psram_free: performance.psram_free,
            wifi_connected: performance.wifi_connected,
            wifi_ssid: performance.wifi_ssid,
            wifi_rssi_dbm: performance.wifi_rssi_dbm,
            station_ip: performance.station_ip,
            ap_active: performance.ap_active,
            ap_ip: performance.ap_ip,
            mqtt_optional_connected: status.mqtt_connected
        }, null, 2);

        text("perf-temperature", sample.dht_valid ? Number(sample.temperature).toFixed(1) + " °C" : "N/A");
        text("perf-humidity", sample.dht_valid ? Number(sample.humidity).toFixed(1) + " %" : "N/A");
        text("perf-luminosity", String(sample.luminosity) + " ADC");
        text("perf-air", String(sample.air_quality) + " ADC");

        renderRows("timings-body", performance.function_timings || [], ["name", "last_us", "avg_us", "max_us"]);
        renderRows("tasks-body", performance.tasks || [], ["name", "stack_high_water_bytes"]);
        drawHistory(history);
    } catch (error) {
        text("perf-status", "Falha: " + error.message);
    }
}

document.getElementById("ota-form")?.addEventListener("submit", async event => {
    event.preventDefault();
    const message = document.getElementById("ota-message");
    const formData = new FormData(event.currentTarget);
    message.hidden = false;
    message.textContent = "Enviando firmware...";

    try {
        const response = await fetch("/api/ota", {
            method: "POST",
            body: formData
        });
        const data = await response.json();
        if (!response.ok) throw new Error(data.error || "Falha no OTA");
        message.textContent = "Firmware atualizado. O ESP32 será reiniciado.";
    } catch (error) {
        message.classList.add("error-message");
        message.textContent = "OTA falhou: " + error.message;
    }
});

document.addEventListener("DOMContentLoaded", () => {
    load();
    setInterval(load, 5000);
});
