import "./navigation.js";
import { getLogsText } from "./api.js";

async function loadLogs() {
    const target = document.getElementById("logs-content");
    try {
        const logs = await getLogsText();
        target.textContent = logs || "Nenhum log persistido ainda.";
    } catch (error) {
        target.textContent = "Falha ao carregar logs: " + error.message;
    }
}

document.addEventListener("DOMContentLoaded", () => {
    document.getElementById("logs-refresh")?.addEventListener("click", loadLogs);
    loadLogs();
});
