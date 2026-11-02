import "./navigation.js";
import {
    controlFan,
    controlExhaust,
    getStatus
} from "./api.js";

function showText(id, value) {
    const element = document.getElementById(id);
    if (element) element.textContent = value;
}

async function refreshStates() {
    try {
        const status = await getStatus();
        showText("fan-state", status.fan_on ? "Ligado" : "Desligado");
        showText("exhaust-state", status.exhaust_on ? "Ligado" : "Desligado");
    } catch {
        showText("message", "Não foi possível consultar o ESP32.");
    }
}

async function sendCommand(actuator, command, button) {
    const buttons = document.querySelectorAll("button[data-actuator][data-command]");
    buttons.forEach(item => item.disabled = true);
    const oldText = button.textContent;
    button.textContent = "Enviando...";

    try {
        if (actuator === "fan") {
            await controlFan(1, command);
        } else {
            await controlExhaust(1, command);
        }
        showText("message", "Comando executado diretamente pelo ESP32.");
        await refreshStates();
    } catch (error) {
        showText("message", "Falha no comando: " + error.message);
    } finally {
        buttons.forEach(item => item.disabled = false);
        button.textContent = oldText;
    }
}

document.addEventListener("DOMContentLoaded", () => {
    document.querySelectorAll("button[data-actuator][data-command]").forEach(button => {
        button.addEventListener("click", () =>
            sendCommand(button.dataset.actuator, button.dataset.command, button)
        );
    });
    refreshStates();
    setInterval(refreshStates, 5000);
});
