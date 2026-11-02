import { getDevices } from "./api.js";

function formatDate(dateString) {
    if (!dateString) {
        return "Ainda não registrada";
    }

    const date = new Date(dateString);

    if (Number.isNaN(date.getTime())) {
        return "Data indisponível";
    }

    return date.toLocaleString("pt-BR", {
        dateStyle: "short",
        timeStyle: "short"
    });
}

function addInfo(container, label, value) {
    const paragraph = document.createElement("p");
    const strong = document.createElement("strong");

    strong.textContent = `${label}: `;
    paragraph.appendChild(strong);
    paragraph.appendChild(
        document.createTextNode(String(value))
    );

    container.appendChild(paragraph);
}

async function loadDevices() {
    const container = document.getElementById("devices-list");

    if (!container) {
        return;
    }

    try {
        const response = await getDevices();

        const devices = Array.isArray(response)
            ? response
            : response.data || [];

        container.innerHTML = "";

        if (devices.length === 0) {
            const message = document.createElement("p");
            message.className = "empty-message";
            message.textContent =
                "Nenhum dispositivo cadastrado.";

            container.appendChild(message);
            return;
        }

        devices.forEach(device => {
            const card = document.createElement("div");
            card.className = "device-item";

            const title = document.createElement("h3");
            title.textContent = `🧩 ${device.name || "Dispositivo"}`;

            const status = document.createElement("p");
            const online =
                String(device.status || "").toLowerCase() === "online";

            status.textContent = online
                ? "● Online"
                : "● Offline";

            status.style.color = online
                ? "#2f6f4f"
                : "#b74b4b";

            status.style.fontWeight = "bold";

            card.appendChild(title);
            card.appendChild(status);

            addInfo(card, "ID", device.id ?? "--");
            addInfo(card, "Código", device.device_code || "--");
            addInfo(
                card,
                "Última comunicação",
                formatDate(device.last_seen)
            );

            container.appendChild(card);
        });

    } catch (error) {
        console.error("Erro ao carregar dispositivos:", error);

        container.textContent =
            "Não foi possível carregar os dispositivos.";
    }
}

document.addEventListener("DOMContentLoaded", () => {
    loadDevices();

    setInterval(loadDevices, 5000);
});
