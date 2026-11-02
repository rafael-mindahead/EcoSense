import "./navigation.js";
import { getAlerts } from "./api.js";

let alerts = [];

function filtered() {
    const filter = document.getElementById("alert-filter")?.value || "all";
    if (filter === "all" || filter === "open") return alerts;
    return [];
}

function counters() {
    document.getElementById("open-alerts-count").textContent = alerts.length;
    document.getElementById("resolved-alerts-count").textContent = "0";
    document.getElementById("total-alerts-count").textContent = alerts.length;
}

function render() {
    const container = document.getElementById("alerts-list-page");
    const message = document.getElementById("alerts-message");
    const rows = filtered();
    container.innerHTML = "";

    if (!rows.length) {
        message.hidden = false;
        message.textContent = "Nenhum warning ou erro persistido no ESP32.";
        document.getElementById("alerts-summary").textContent = "0 eventos";
        return;
    }

    message.hidden = true;
    document.getElementById("alerts-summary").textContent =
        rows.length + " evento(s) persistido(s)";

    rows.slice().reverse().forEach(alert => {
        const item = document.createElement("article");
        item.className = "alert-page-item";

        const icon = document.createElement("div");
        icon.className = "alert-page-icon";
        icon.textContent = alert.level === "ERROR" ? "⛔" : "⚠️";

        const content = document.createElement("div");
        content.className = "alert-page-content";

        const title = document.createElement("h4");
        title.textContent = alert.level === "ERROR" ? "Erro" : "Warning";

        const body = document.createElement("p");
        body.textContent = alert.message || "Evento registrado";

        const meta = document.createElement("small");
        meta.textContent = alert.created_at || "--";

        content.append(title, body, meta);
        item.append(icon, content);
        container.appendChild(item);
    });
}

async function load() {
    const message = document.getElementById("alerts-message");
    message.hidden = false;
    message.textContent = "Carregando eventos do ESP32...";

    try {
        alerts = await getAlerts();
        counters();
        render();
    } catch (error) {
        message.hidden = false;
        message.classList.add("error-message");
        message.textContent = "Falha ao carregar logs: " + error.message;
    }
}

document.addEventListener("DOMContentLoaded", () => {
    document.getElementById("alert-filter")?.addEventListener("change", render);
    document.getElementById("alerts-refresh")?.addEventListener("click", load);
    load();
});
