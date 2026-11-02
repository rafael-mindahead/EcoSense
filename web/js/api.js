const API_BASE_URL = "";

async function request(endpoint, options = {}) {
    const response = await fetch(API_BASE_URL + endpoint, options);
    const contentType = response.headers.get("content-type") || "";
    const data = contentType.includes("application/json")
        ? await response.json()
        : await response.text();

    if (!response.ok) {
        const message = typeof data === "object"
            ? data.error || data.message || "Erro ao acessar o ESP32"
            : data || "Erro ao acessar o ESP32";
        throw new Error(message);
    }

    return data;
}

function postForm(endpoint, values) {
    return request(endpoint, {
        method: "POST",
        headers: {
            "Content-Type": "application/x-www-form-urlencoded"
        },
        body: new URLSearchParams(values)
    });
}

function sampleToMeasurement(sample) {
    return {
        ...sample,
        device_id: 1,
        created_at: sample.epoch
            ? new Date(Number(sample.epoch) * 1000).toISOString()
            : null
    };
}

export async function getApiHealth() {
    const status = await request("/api/status");
    return {
        service: "ESP32 Web Server",
        status: status.wifi_connected || status.ap_active ? "online" : "starting"
    };
}

export function getStatus() {
    return request("/api/status");
}

export async function getLatestMeasurement() {
    const sample = await request("/api/sensors/latest");
    return sampleToMeasurement(sample);
}

export async function getMeasurements() {
    const samples = await request("/api/sensors/history");
    return (Array.isArray(samples) ? samples : []).map(sampleToMeasurement);
}

export async function getDevices() {
    const status = await getStatus();
    return [{
        id: status.device_id || 1,
        name: "EcoSense ESP32",
        device_code: "ESP32-" + String(status.device_id || 1),
        status: status.wifi_connected || status.ap_active ? "online" : "offline",
        last_seen: new Date().toISOString(),
        station_ip: status.station_ip,
        ap_ip: status.ap_ip,
        rssi_dbm: status.rssi_dbm,
        mqtt_connected: status.mqtt_connected,
        version: status.version
    }];
}

export async function getAlerts() {
    const text = await request("/api/logs");
    const rows = String(text).split("\n").filter(Boolean);
    return rows
        .map((line, index) => {
            const [timestamp, level, ...message] = line.split("|");
            return {
                id: index + 1,
                status: "open",
                level,
                metric: level === "ERROR" ? "Erro" : "Aviso",
                message: message.join("|"),
                created_at: timestamp
            };
        })
        .filter(item => item.level === "WARNING" || item.level === "ERROR");
}

export async function resolveAlert() {
    throw new Error("Alertas locais são eventos persistentes e não são removidos.");
}

export function controlFan(_deviceId, command) {
    return postForm("/api/actuator", {
        actuator: "fan",
        command
    });
}

export function controlExhaust(_deviceId, command) {
    return postForm("/api/actuator", {
        actuator: "exhaust",
        command
    });
}

export function getPerformance() {
    return request("/api/performance");
}

export function getConfig() {
    return request("/api/config");
}

export function saveSensorInterval(intervalMs) {
    return postForm("/api/config/sample", {
        interval_ms: String(intervalMs)
    });
}

export function saveWifi(ssid, password) {
    return postForm("/api/config/wifi", { ssid, password });
}

export function saveMqttHost(host) {
    return postForm("/api/config/mqtt", { host });
}

export function requestLightSleep(seconds) {
    return postForm("/api/energy/light-sleep", {
        seconds: String(seconds)
    });
}

export function getLogsText() {
    return request("/api/logs");
}
