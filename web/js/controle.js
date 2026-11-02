
import {
    controlFan,
    controlExhaust
} from "./api.js";

function showText(elementId, value) {
    const element = document.getElementById(elementId);

    if (element) {
        element.textContent = value;
    }
}

function setButtonsDisabled(disabled) {
    const buttons = document.querySelectorAll(
        "button[data-actuator][data-command]"
    );

    buttons.forEach((button) => {
        button.disabled = disabled;
    });
}

async function sendCommand(actuator, command, button) {
    const inputId = actuator === "fan"
        ? "fan-id"
        : "exhaust-id";

    const statusId = actuator === "fan"
        ? "fan-state"
        : "exhaust-state";

    const deviceId = Number(
        document.getElementById(inputId).value
    );

    if (!Number.isInteger(deviceId) || deviceId < 1) {
        showText(
            "message",
            "Informe um ID de dispositivo válido."
        );
        return;
    }

    setButtonsDisabled(true);

    showText(statusId, "Enviando comando...");
    showText("message", "");

    try {
        if (actuator === "fan") {
            await controlFan(deviceId, command);
        } else if (actuator === "exhaust") {
            await controlExhaust(deviceId, command);
        }

        const action = command === "ON"
            ? "ligar"
            : "desligar";

        const name = actuator === "fan"
            ? "ventilador"
            : "exaustor";

        showText(statusId, "Comando enviado");

        showText(
            "message",
            `Comando para ${action} o ${name} recebido pela API.`
        );

    } catch (error) {
        console.error("Erro ao enviar comando:", error);

        showText(statusId, "Falha no envio");

        showText(
            "message",
            `Não foi possível enviar o comando: ${error.message}`
        );

    } finally {
        setButtonsDisabled(false);
    }
}

function setupControlButtons() {
    const buttons = document.querySelectorAll(
        "button[data-actuator][data-command]"
    );

    buttons.forEach((button) => {
        button.addEventListener("click", () => {
            const actuator = button.dataset.actuator;
            const command = button.dataset.command;

            sendCommand(actuator, command, button);
        });
    });
}

document.addEventListener("DOMContentLoaded", () => {
    setupControlButtons();
});
