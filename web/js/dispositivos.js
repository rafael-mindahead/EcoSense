import "./navigation.js";
import { getDevices } from "./api.js";

function addInfo(container, label, value) {
    const paragraph = document.createElement("p");
    const strong = document.createElement("strong");
    strong.textContent = label + ": ";
    paragraph.appendChild(strong);
    paragraph.appendChild(document.createTextNode(String(value ?? "--")));
    container.appendChild(paragraph);
}

async function loadDevices() {
    const container = document.getElementById("devices-list");
    if (!container) return;

    try {
        const devices = await getDevices();
        container.innerHTML = "";

        devices.forEach(device => {
            const card = document.createElement("div");
            card.className = "device-item";

            const title = document.createElement("h3");
            title.textContent = "🧩 " + device.name;

            const status = document.createElement("p");
            status.textContent = device.status === "online" ? "● Online" : "● Offline";
            status.style.color = device.status === "online" ? "#2f6f4f" : "#b74b4b";
            status.style.fontWeight = "bold";

            card.append(title, status);
            addInfo(card, "ID", device.id);
            addInfo(card, "Firmware", device.version);
            addInfo(card, "IP da estação", device.station_ip || "--");
            addInfo(card, "IP do AP", device.ap_ip || "--");
            addInfo(card, "RSSI", String(device.rssi_dbm ?? "--") + " dBm");
            addInfo(card, "MQTT opcional", device.mqtt_connected ? "Conectado" : "Desconectado");
            container.appendChild(card);
        });
    } catch (error) {
        container.textContent = "Não foi possível carregar o ESP32: " + error.message;
    }
}

document.addEventListener("DOMContentLoaded", () => {
    loadDevices();
    setInterval(loadDevices, 5000);
});
