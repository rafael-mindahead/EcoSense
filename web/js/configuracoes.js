import "./navigation.js";
import {
    getConfig,
    saveSensorInterval,
    saveWifi,
    saveMqttHost,
    requestLightSleep
} from "./api.js";

const message = document.getElementById("message");

function feedback(text, error = false) {
    message.textContent = text;
    message.classList.toggle("error-message", error);
}

async function load() {
    try {
        const config = await getConfig();
        document.getElementById("sample-ms").value = config.sensor_interval_ms;
        document.getElementById("sample-ms").min = config.min_sensor_interval_ms;
        document.getElementById("sample-ms").max = config.max_sensor_interval_ms;
        document.getElementById("ssid").value = config.wifi_ssid || "";
        document.getElementById("mqtt-host").value = config.mqtt_host || "";
    } catch (error) {
        feedback("Não foi possível carregar a configuração: " + error.message, true);
    }
}

document.getElementById("sample-form")?.addEventListener("submit", async event => {
    event.preventDefault();
    try {
        await saveSensorInterval(Number(document.getElementById("sample-ms").value));
        feedback("Intervalo de leitura salvo.");
    } catch (error) {
        feedback(error.message, true);
    }
});

document.getElementById("wifi-form")?.addEventListener("submit", async event => {
    event.preventDefault();
    try {
        await saveWifi(
            document.getElementById("ssid").value.trim(),
            document.getElementById("wifi-password").value
        );
        feedback("Wi-Fi salvo. O ESP32 tentará a nova rede; o navegador pode perder a conexão.");
    } catch (error) {
        feedback(error.message, true);
    }
});

document.getElementById("mqtt-form")?.addEventListener("submit", async event => {
    event.preventDefault();
    try {
        await saveMqttHost(document.getElementById("mqtt-host").value.trim());
        feedback("Broker MQTT opcional atualizado.");
    } catch (error) {
        feedback(error.message, true);
    }
});

document.getElementById("sleep-form")?.addEventListener("submit", async event => {
    event.preventDefault();
    try {
        await requestLightSleep(Number(document.getElementById("sleep-seconds").value));
        feedback("Light Sleep agendado. A conexão será retomada após o despertar.");
    } catch (error) {
        feedback(error.message, true);
    }
});

document.addEventListener("DOMContentLoaded", load);
