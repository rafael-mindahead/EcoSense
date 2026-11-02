const items = [
    ["dashboard", "🏠", "Dashboard"],
    ["historico", "📊", "Histórico"],
    ["performance", "⚡", "Performance"],
    ["logs", "🧾", "Logs"],
    ["alertas", "🔔", "Alertas"],
    ["dispositivos", "⚙️", "Dispositivos"],
    ["controle", "🌀", "Controle"],
    ["configuracoes", "🔧", "Configurações"],
    ["sobre", "ℹ️", "Sobre"],
    ["changelog", "📝", "Changelog"]
];

function renderNavigation() {
    const menu = document.querySelector(".menu");
    if (!menu) return;

    const current = location.pathname === "/"
        ? "dashboard"
        : location.pathname.replace(/^\/+|\/+$/g, "");

    menu.innerHTML = items.map(([route, icon, label]) => {
        const active = current === route ? " active" : "";
        return '<a href="/' + route + '" class="menu-item' + active + '">' +
            '<span>' + icon + '</span>' + label + '</a>';
    }).join("");
}

if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", renderNavigation);
} else {
    renderNavigation();
}
