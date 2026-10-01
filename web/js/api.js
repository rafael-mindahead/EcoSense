const API_BASE_URL = "/api";

async function apiRequest(endpoint, options = {}) {
    try {
        const response = await fetch(
            `${API_BASE_URL}${endpoint}`,
            {
                headers: {
                    "Content-Type": "application/json",
                    ...(options.headers || {})
                },
                ...options
            }
        );

        const data = await response.json();

        if (!response.ok) {
            throw new Error(
                data.error ||
                data.message ||
                "Erro ao acessar a API"
            );
        }

        return data;

    } catch (error) {
        console.error(
            "Erro na comunicação com a API:",
            error
        );

        throw error;
    }
}

export async function getApiHealth() {
    return apiRequest("/health");
}

export async function getLatestMeasurement() {
    return apiRequest(
        "/v1/meansurements/latest"
    );
}

export async function getMeasurements(
    limit = 50
) {
    return apiRequest(
        `/v1/meansurements?limit=${limit}`
    );
}

export async function getDevices() {
    return apiRequest(
        "/v1/devices"
    );
}

export async function getAlerts() {
    return apiRequest(
        "/v1/alerts"
    );
}

export async function resolveAlert(
    alertId
) {
    return apiRequest(
        "/v1/alerts/resolve",
        {
            method: "POST",

            body: JSON.stringify({
                alert_id: alertId
            })
        }
    );
}

export async function controlFan(
    deviceId,
    command
) {
    return apiRequest(
        "/v1/actuators/fan",
        {
            method: "POST",

            body: JSON.stringify({
                device_id: deviceId,
                command: command
            })
        }
    );
}

export async function controlExhaust(
    deviceId,
    command
) {
    return apiRequest(
        "/v1/actuators/exhaust",
        {
            method: "POST",

            body: JSON.stringify({
                device_id: deviceId,
                command: command
            })
        }
    );
}


export async function getSensorLimits() {
    return apiRequest(
        "/v1/sensor-limits"
    );
}