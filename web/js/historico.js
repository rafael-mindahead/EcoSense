import "./navigation.js";
import { getMeasurements, getDevices } from "./api.js";

let measurements = [];
let devices = [];

const fmt = value => {
    if (!value) return "--";
    const date = new Date(value);
    return Number.isNaN(date.getTime())
        ? value
        : date.toLocaleString("pt-BR", {
            dateStyle: "short",
            timeStyle: "short"
        });
};

const num = (value, decimals = 1) =>
    Number.isFinite(Number(value)) ? Number(value).toFixed(decimals) : "--";

function drawSeries(canvasId, series) {
    const canvas = document.getElementById(canvasId);
    if (!canvas) return;

    const ratio = window.devicePixelRatio || 1;
    const width = Math.max(canvas.clientWidth, 320);
    const height = Math.max(canvas.clientHeight, 260);
    canvas.width = width * ratio;
    canvas.height = height * ratio;

    const ctx = canvas.getContext("2d");
    ctx.scale(ratio, ratio);
    ctx.clearRect(0, 0, width, height);

    const allValues = series.flatMap(item => item.values).filter(Number.isFinite);
    if (!allValues.length) {
        ctx.fillStyle = "#68736d";
        ctx.font = "14px sans-serif";
        ctx.fillText("Sem histórico disponível", 20, 30);
        return;
    }

    const min = Math.min(...allValues);
    const max = Math.max(...allValues);
    const range = Math.max(max - min, 1);
    const padding = 26;
    const tones = ["#2f6f4f", "#6fa7c8", "#d8b04c"];

    series.forEach((item, seriesIndex) => {
        ctx.strokeStyle = tones[seriesIndex % tones.length];
        ctx.lineWidth = 2;
        ctx.beginPath();

        item.values.forEach((value, index) => {
            if (!Number.isFinite(value)) return;
            const x = padding + index * ((width - padding * 2) /
                Math.max(item.values.length - 1, 1));
            const y = height - padding - ((value - min) / range) *
                (height - padding * 2);
            if (index === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        });

        ctx.stroke();
        ctx.fillStyle = tones[seriesIndex % tones.length];
        ctx.font = "12px sans-serif";
        ctx.fillText(item.label, padding + seriesIndex * 130, 16);
    });
}

function renderCharts(rows) {
    drawSeries("environment-chart", [
        {
            label: "Temperatura °C",
            values: rows.map(item => item.dht_valid ? Number(item.temperature) : NaN)
        },
        {
            label: "Umidade %",
            values: rows.map(item => item.dht_valid ? Number(item.humidity) : NaN)
        }
    ]);

    drawSeries("luminosity-chart", [{
        label: "Luminosidade ADC",
        values: rows.map(item => Number(item.luminosity))
    }]);

    drawSeries("air-chart", [{
        label: "Qualidade do ar ADC",
        values: rows.map(item => Number(item.air_quality))
    }]);
}

function renderTable(rows) {
    const message = document.getElementById("history-message");
    const wrapper = document.getElementById("history-table-wrapper");
    const body = document.getElementById("history-table-body");
    body.innerHTML = "";

    if (!rows.length) {
        wrapper.hidden = true;
        message.hidden = false;
        message.textContent = "Ainda não há histórico armazenado no ESP32.";
        document.getElementById("history-summary").textContent = "0 medições";
        return;
    }

    message.hidden = true;
    wrapper.hidden = false;
    document.getElementById("history-summary").textContent =
        rows.length + " ponto(s) das últimas 24 horas";

    rows.slice().reverse().forEach(item => {
        const tr = document.createElement("tr");
        const values = [
            fmt(item.created_at),
            devices[0]?.name || "EcoSense ESP32",
            item.dht_valid ? num(item.temperature) + " °C" : "N/A",
            item.dht_valid ? num(item.humidity) + " %" : "N/A",
            num(item.luminosity, 0) + " ADC",
            num(item.air_quality, 0) + " ADC"
        ];

        values.forEach(value => {
            const td = document.createElement("td");
            td.textContent = value;
            tr.appendChild(td);
        });
        body.appendChild(tr);
    });
}

function render() {
    renderCharts(measurements);
    renderTable(measurements);
}

async function load() {
    const message = document.getElementById("history-message");
    message.hidden = false;
    message.textContent = "Carregando histórico local de 24 horas...";

    try {
        [measurements, devices] = await Promise.all([
            getMeasurements(),
            getDevices()
        ]);
        render();
    } catch (error) {
        message.classList.add("error-message");
        message.textContent = "Falha ao carregar histórico: " + error.message;
        document.getElementById("history-table-wrapper").hidden = true;
    }
}

document.addEventListener("DOMContentLoaded", () => {
    document.getElementById("history-device")?.closest(".field")?.remove();
    document.getElementById("history-refresh")?.addEventListener("click", load);
    load();
});
