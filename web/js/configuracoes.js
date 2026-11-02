import { getSensorLimits, saveSensorLimit } from "./api.js";

// =====================================
// ELEMENTOS DA PÁGINA
// =====================================

const form = document.querySelector("#limit-form");
const list = document.querySelector("#limits");
const msg = document.querySelector("#message");

// =====================================
// PROTEÇÃO DOS DADOS EXIBIDOS
// =====================================

// Evita que caracteres especiais sejam interpretados como HTML
const esc = (value) => {
    return String(value ?? "—").replace(
        /[&<>"']/g,
        (character) => ({
            "&": "&amp;",
            "<": "&lt;",
            ">": "&gt;",
            '"': "&quot;",
            "'": "&#39;"
        }[character])
    );
};

// =====================================
// CARREGAR LIMITES DA API
// =====================================

async function load() {
    try {
        // Consulta os limites cadastrados
        const response = await getSensorLimits();

        const rows = response.data || [];

        // Exibe a tabela quando existem registros
        if (rows.length) {
            list.innerHTML = `
                <table>
                    <thead>
                        <tr>
                            <th>Dispositivo</th>
                            <th>Métrica</th>
                            <th>Mínimo</th>
                            <th>Máximo</th>
                            <th>Ativo</th>
                        </tr>
                    </thead>

                    <tbody>
                        ${rows.map((item) => `
                            <tr>
                                <td>${esc(item.device_id)}</td>
                                <td>${esc(item.metric)}</td>
                                <td>${esc(item.min_value)}</td>
                                <td>${esc(item.max_value)}</td>
                                <td>${item.enabled ? "Sim" : "Não"}</td>
                            </tr>
                        `).join("")}
                    </tbody>
                </table>
            `;
        } else {
            list.textContent = "Nenhum limite cadastrado.";
        }

    } catch (error) {
        console.error("Erro ao consultar limites:", error);

        list.textContent =
            "Não foi possível consultar os limites da API.";
    }
}

// =====================================
// SALVAR LIMITE AMBIENTAL
// =====================================

form.addEventListener("submit", async (event) => {
    // Evita que a página recarregue
    event.preventDefault();

    // Obtém os dados preenchidos no formulário
    const data = new FormData(form);

    const min = data.get("min_value");
    const max = data.get("max_value");

    // Valida o intervalo informado
    if (
        min !== "" &&
        max !== "" &&
        Number(min) > Number(max)
    ) {
        msg.textContent =
            "O mínimo não pode ser maior que o máximo.";

        return;
    }

    try {
        // Envia os dados para a API
        await saveSensorLimit({
            device_id: Number(data.get("device_id")),
            metric: data.get("metric"),
            min_value: min === "" ? null : Number(min),
            max_value: max === "" ? null : Number(max),
            enabled: form.elements.enabled.checked
        });

        // Mensagem de sucesso
        msg.textContent = "Limite salvo.";

        // Atualiza a tabela com os dados cadastrados
        await load();

    } catch (error) {
        console.error("Erro ao salvar limite:", error);

        msg.textContent =
            "Não foi possível salvar. Confirme a disponibilidade da API e os dados.";
    }
});

// =====================================
// INICIALIZAÇÃO
// =====================================

// Carrega os limites quando a página é aberta
load();
