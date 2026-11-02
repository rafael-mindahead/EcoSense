import { getAlerts, resolveAlert } from "./api.js";

// =====================================
// VARIÁVEIS GLOBAIS
// =====================================

let alerts = [];

// =====================================
// FORMATAÇÃO DOS DADOS
// =====================================

// Formata a data e a hora no padrão brasileiro
const fmt = (value) => {
    if (!value) {
        return "--";
    }

    const date = new Date(value);

    return Number.isNaN(date.getTime())
        ? value
        : date.toLocaleString("pt-BR", {
            dateStyle: "short",
            timeStyle: "short"
        });
};

// Traduz os nomes das métricas
const label = (metric) => {
    const metrics = {
        temperature: "Temperatura",
        humidity: "Umidade",
        luminosity: "Luminosidade",
        air_quality: "Qualidade do ar"
    };

    return metrics[metric] || metric || "Sensor";
};

// =====================================
// FILTRO DE ALERTAS
// =====================================

function filtered() {
    const filter = document.getElementById("alert-filter").value;

    if (filter === "all") {
        return alerts;
    }

    return alerts.filter((alert) => alert.status === filter);
}

// =====================================
// CONTADORES DE ALERTAS
// =====================================

function counters() {
    const open = alerts.filter(
        (alert) => alert.status === "open"
    ).length;

    const resolved = alerts.filter(
        (alert) => alert.status === "resolved"
    ).length;

    document.getElementById("open-alerts-count").textContent = open;

    document.getElementById("resolved-alerts-count").textContent = resolved;

    document.getElementById("total-alerts-count").textContent = alerts.length;

    document.getElementById("alert-count").textContent = open;
}

// =====================================
// EXIBIÇÃO DOS ALERTAS
// =====================================

function render() {
    const container = document.getElementById("alerts-list-page");
    const message = document.getElementById("alerts-message");

    const rows = filtered();

    container.innerHTML = "";

    // Nenhum alerta encontrado
    if (!rows.length) {
        message.hidden = false;
        message.classList.remove("error-message");

        message.textContent =
            "Nenhum alerta encontrado para o filtro selecionado.";

        document.getElementById("alerts-summary").textContent =
            "0 alertas";

        return;
    }

    message.hidden = true;

    document.getElementById("alerts-summary").textContent =
        `${rows.length} alerta(s) exibido(s)`;

    // Cria os cards de alerta
    rows.forEach((alert) => {
        const item = document.createElement("article");

        item.className = `alert-page-item ${
            alert.status === "resolved" ? "is-resolved" : ""
        }`;

        // Ícone
        const icon = document.createElement("div");

        icon.className = "alert-page-icon";

        icon.textContent =
            alert.status === "resolved" ? "✓" : "⚠️";

        // Conteúdo
        const content = document.createElement("div");

        content.className = "alert-page-content";

        // Título
        const title = document.createElement("h4");

        title.textContent = label(alert.metric);

        // Descrição
        const body = document.createElement("p");

        body.textContent = alert.message || "Alerta detectado";

        // Informações adicionais
        const meta = document.createElement("small");

        meta.textContent =
            `Dispositivo ${alert.device_id ?? "--"} ` +
            `• Valor: ${alert.value ?? "--"} ` +
            `• Criado em ${fmt(alert.created_at)}` +
            (
                alert.resolved_at
                    ? ` • Resolvido em ${fmt(alert.resolved_at)}`
                    : ""
            );

        content.append(title, body, meta);

        // Ações do alerta
        const actions = document.createElement("div");

        actions.className = "alert-actions";

        // Status
        const status = document.createElement("span");

        status.className = `status-pill ${alert.status}`;

        status.textContent =
            alert.status === "open" ? "Aberto" : "Resolvido";

        actions.appendChild(status);

        // Botão Resolver, somente para alertas abertos
        if (alert.status === "open") {
            const button = document.createElement("button");

            button.type = "button";
            button.className = "btn btn-danger";
            button.textContent = "Resolver";

            button.addEventListener("click", () => {
                handleResolve(alert.id, button);
            });

            actions.appendChild(button);
        }

        item.append(icon, content, actions);

        container.appendChild(item);
    });
}

// =====================================
// RESOLVER ALERTA
// =====================================

async function handleResolve(id, button) {
    button.disabled = true;
    button.textContent = "Resolvendo...";

    try {
        // Envia a solicitação para a API
        await resolveAlert(id);

        // Atualiza o alerta local
        const alert = alerts.find(
            (item) => Number(item.id) === Number(id)
        );

        if (alert) {
            alert.status = "resolved";
            alert.resolved_at = new Date().toISOString();
        }

        // Atualiza contadores e lista
        counters();
        render();

    } catch (error) {
        button.disabled = false;
        button.textContent = "Resolver";

        window.alert(
            `Não foi possível resolver o alerta: ${error.message}`
        );
    }
}

// =====================================
// CARREGAR ALERTAS DA API
// =====================================

async function load() {
    const message = document.getElementById("alerts-message");

    message.hidden = false;
    message.classList.remove("error-message");

    message.textContent = "Carregando alertas...";

    try {
        // Busca alertas no backend
        const response = await getAlerts();

        alerts = Array.isArray(response)
            ? response
            : response.data || [];

        // Atualiza a página
        counters();
        render();

    } catch (error) {
        message.hidden = false;
        message.classList.add("error-message");

        message.textContent =
            `Não foi possível carregar os alertas: ${error.message}`;
    }
}

// =====================================
// INICIALIZAÇÃO DA PÁGINA
// =====================================

document.addEventListener("DOMContentLoaded", () => {
    // Atualiza a lista ao mudar o filtro
    document.getElementById("alert-filter")
        .addEventListener("change", render);

    // Atualiza a lista ao clicar no botão
    document.getElementById("alerts-refresh")
        .addEventListener("click", load);

    // Carrega os alertas ao abrir a página
    load();
});
