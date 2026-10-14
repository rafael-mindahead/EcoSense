#include "WebServerService.h"

#include <ArduinoJson.h>
#include <LittleFS.h>

#include "AppConfig.h"
#include "WebAssets.h"

WebServerService::WebServerService(
    ConfigService& config,
    WifiService& wifi,
    TimeService& time,
    SensorService& sensors,
    HistoryService& history,
    LogService& logs,
    PerformanceService& performance,
    ActuatorHardwareService& actuators,
    MqttDeviceService& mqtt,
    EnergyService& energy,
    OtaService& ota
) :
    config(config),
    wifi(wifi),
    time(time),
    sensors(sensors),
    history(history),
    logs(logs),
    performance(performance),
    actuators(actuators),
    mqtt(mqtt),
    energy(energy),
    ota(ota),
    server(EcoSenseConfig::HTTP_PORT)
{
}

void WebServerService::begin()
{
    registerRoutes();

    server.begin();

    logs.info(
        "Servidor HTTP embarcado iniciado"
    );
}

void WebServerService::tick()
{
    const uint32_t started =
        micros();

    server.handleClient();

    const uint32_t duration =
        micros() - started;

    performance.recordTiming(
        "web_handle",
        duration
    );

    performance.addBusyMicros(
        duration
    );

    if (
        restartRequested &&
        static_cast<int32_t>(
            millis() -
            restartAtMs
        ) >= 0
    ) {
        ESP.restart();
    }
}

void WebServerService::registerRoutes()
{
    server.on(
        "/",
        HTTP_GET,
        [this]() {
            server.send_P(
                200,
                "text/html; charset=utf-8",
                ECOSENSE_WEB_APP
            );
        }
    );

    server.on(
        "/performance",
        HTTP_GET,
        [this]() {
            server.sendHeader(
                "Location",
                "/#performance"
            );

            server.send(
                302,
                "text/plain",
                ""
            );
        }
    );

    server.on(
        "/api/status",
        HTTP_GET,
        [this]() {
            sendJson(
                200,
                statusJson()
            );
        }
    );

    server.on(
        "/api/sensors/latest",
        HTTP_GET,
        [this]() {
            sendJson(
                200,
                latestSensorJson()
            );
        }
    );

    server.on(
        "/api/sensors/history",
        HTTP_GET,
        [this]() {
            sendJson(
                200,
                history
                    .readLast24HoursJson()
            );
        }
    );

    server.on(
        "/api/performance",
        HTTP_GET,
        [this]() {
            sendJson(
                200,
                performance
                    .snapshotJson()
            );
        }
    );

    server.on(
        "/api/config",
        HTTP_GET,
        [this]() {
            sendJson(
                200,
                configJson()
            );
        }
    );

    server.on(
        "/api/config/sample",
        HTTP_POST,
        [this]() {
            if (
                !server.hasArg(
                    "interval_ms"
                )
            ) {
                sendError(
                    422,
                    "interval_ms is required"
                );
                return;
            }

            const uint32_t interval =
                server.arg(
                    "interval_ms"
                ).toInt();

            if (
                interval <
                    EcoSenseConfig::
                        MIN_SENSOR_INTERVAL_MS ||
                interval >
                    EcoSenseConfig::
                        MAX_SENSOR_INTERVAL_MS
            ) {
                sendError(
                    422,
                    "invalid sensor interval"
                );
                return;
            }

            config.saveSensorInterval(
                interval
            );

            logs.info(
                "Intervalo de sensores alterado para "
                + String(interval)
                + " ms"
            );

            sendJson(
                200,
                "{\"status\":\"saved\"}"
            );
        }
    );

    server.on(
        "/api/config/mqtt",
        HTTP_POST,
        [this]() {
            if (
                !server.hasArg("host") ||
                server.arg("host").isEmpty()
            ) {
                sendError(
                    422,
                    "host is required"
                );
                return;
            }

            config.saveMqttHost(
                server.arg("host")
            );

            mqtt.reloadConfiguration();

            logs.info(
                "Host MQTT atualizado"
            );

            sendJson(
                200,
                "{\"status\":\"saved\"}"
            );
        }
    );

    server.on(
        "/api/config/wifi",
        HTTP_POST,
        [this]() {
            if (
                !server.hasArg("ssid") ||
                server.arg("ssid").isEmpty()
            ) {
                sendError(
                    422,
                    "ssid is required"
                );
                return;
            }

            const String password =
                server.hasArg("password")
                    ? server.arg("password")
                    : "";

            wifi.reconfigure(
                server.arg("ssid"),
                password
            );

            logs.info(
                "Configuracao Wi-Fi atualizada via Web"
            );

            sendJson(
                200,
                "{\"status\":\"reconnecting\"}"
            );
        }
    );

    server.on(
        "/api/actuator",
        HTTP_POST,
        [this]() {
            if (
                !server.hasArg("actuator") ||
                !server.hasArg("command")
            ) {
                sendError(
                    422,
                    "actuator and command are required"
                );
                return;
            }

            const uint32_t started =
                micros();

            String error;

            const bool success =
                actuators.execute(
                    server.arg("actuator"),
                    server.arg("command"),
                    error
                );

            const uint32_t duration =
                micros() - started;

            performance.recordTiming(
                "actuator_execute",
                duration
            );

            performance.addBusyMicros(
                duration
            );

            if (!success) {
                sendError(
                    422,
                    error
                );
                return;
            }

            logs.info(
                "Atuador controlado via Web: "
                + server.arg("actuator")
                + " "
                + server.arg("command")
            );

            sendJson(
                200,
                "{\"status\":\"executed\"}"
            );
        }
    );

    server.on(
        "/api/energy/light-sleep",
        HTTP_POST,
        [this]() {
            if (
                !server.hasArg("seconds")
            ) {
                sendError(
                    422,
                    "seconds is required"
                );
                return;
            }

            const uint32_t seconds =
                server.arg("seconds")
                    .toInt();

            if (
                !energy.requestLightSleep(
                    seconds
                )
            ) {
                sendError(
                    422,
                    "invalid sleep request"
                );
                return;
            }

            sendJson(
                202,
                "{\"status\":\"light_sleep_scheduled\"}"
            );
        }
    );

    server.on(
        "/api/logs",
        HTTP_GET,
        [this]() {
            server.send(
                200,
                "text/plain; charset=utf-8",
                logs.readTail(100)
            );
        }
    );

    server.on(
        "/api/logs/export",
        HTTP_GET,
        [this]() {
            File file =
                LittleFS.open(
                    logs.filePath(),
                    FILE_READ
                );

            if (!file) {
                sendError(
                    404,
                    "log file not found"
                );
                return;
            }

            server.sendHeader(
                "Content-Disposition",
                "attachment; filename=ecosense.log"
            );

            server.streamFile(
                file,
                "text/plain"
            );

            file.close();
        }
    );

    server.on(
        "/api/ota",
        HTTP_POST,
        [this]() {
            const bool success =
                !ota.hasError();

            server.send(
                success ? 200 : 500,
                "application/json",
                success
                    ? "{\"status\":\"updated\",\"restart\":true}"
                    : "{\"error\":\"ota failed\"}"
            );

            if (success) {
                restartRequested =
                    true;

                restartAtMs =
                    millis() + 1000;
            }
        },
        [this]() {
            HTTPUpload& upload =
                server.upload();

            if (
                upload.status ==
                UPLOAD_FILE_START
            ) {
                ota.beginUpdate();
            } else if (
                upload.status ==
                UPLOAD_FILE_WRITE
            ) {
                ota.writeChunk(
                    upload.buf,
                    upload.currentSize
                );
            } else if (
                upload.status ==
                UPLOAD_FILE_END
            ) {
                ota.finishUpdate();
            } else if (
                upload.status ==
                UPLOAD_FILE_ABORTED
            ) {
                logs.error(
                    "OTA: upload abortado"
                );
            }
        }
    );

    server.onNotFound(
        [this]() {
            sendError(
                404,
                "route not found"
            );
        }
    );
}

void WebServerService::sendJson(
    int status,
    const String& json
)
{
    server.send(
        status,
        "application/json",
        json
    );
}

void WebServerService::sendError(
    int status,
    const String& message
)
{
    StaticJsonDocument<192> document;

    document["error"] =
        message;

    String json;

    serializeJson(
        document,
        json
    );

    sendJson(
        status,
        json
    );
}

String WebServerService::
    latestSensorJson() const
{
    const SensorSample sample =
        sensors.latest();

    StaticJsonDocument<256> document;

    document["epoch"] =
        static_cast<long>(
            sample.epoch
        );

    document["uptime_ms"] =
        sample.uptimeMs;

    document["dht_valid"] =
        sample.dhtValid;

    if (sample.dhtValid) {
        document["temperature"] =
            sample.temperature;

        document["humidity"] =
            sample.humidity;
    } else {
        document["temperature"] =
            nullptr;

        document["humidity"] =
            nullptr;
    }

    document["luminosity"] =
        sample.luminosity;

    document["air_quality"] =
        sample.airQuality;

    String json;

    serializeJson(
        document,
        json
    );

    return json;
}

String WebServerService::statusJson() const
{
    StaticJsonDocument<512> document;

    document["version"] =
        EcoSenseConfig::
            PROJECT_VERSION;

    document["device_id"] =
        EcoSenseConfig::
            DEVICE_ID;

    document["uptime_ms"] =
        millis();

    document["wifi_connected"] =
        wifi.isConnected();

    document["wifi_ssid"] =
        wifi.ssid();

    document["rssi_dbm"] =
        wifi.rssi();

    document["station_ip"] =
        wifi.stationIp();

    document["ap_active"] =
        wifi.isAccessPointActive();

    document["ap_ip"] =
        wifi.accessPointIp();

    document["time_synchronized"] =
        time.isSynchronized();

    document["mqtt_connected"] =
        mqtt.isConnected();

    document["fan_on"] =
        actuators.fanOn();

    document["exhaust_on"] =
        actuators.exhaustOn();

    String json;

    serializeJson(
        document,
        json
    );

    return json;
}

String WebServerService::configJson() const
{
    StaticJsonDocument<384> document;

    document["wifi_ssid"] =
        config.wifiSsid();

    document["mqtt_host"] =
        config.mqttHost();

    document["sensor_interval_ms"] =
        config.sensorIntervalMs();

    document["min_sensor_interval_ms"] =
        EcoSenseConfig::
            MIN_SENSOR_INTERVAL_MS;

    document["max_sensor_interval_ms"] =
        EcoSenseConfig::
            MAX_SENSOR_INTERVAL_MS;

    String json;

    serializeJson(
        document,
        json
    );

    return json;
}
