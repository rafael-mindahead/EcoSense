#include "MqttDeviceService.h"

#include <ArduinoJson.h>

#include "AppConfig.h"

MqttDeviceService*
    MqttDeviceService::instance =
        nullptr;

MqttDeviceService::MqttDeviceService(
    ConfigService& config,
    WifiService& wifi,
    ActuatorHardwareService& actuators,
    LogService& logs,
    PerformanceService& performance
) :
    config(config),
    wifi(wifi),
    actuators(actuators),
    logs(logs),
    performance(performance),
    client(wifiClient)
{
    instance = this;
}

void MqttDeviceService::begin()
{
    reloadConfiguration();

    client.setCallback(
        MqttDeviceService::callback
    );

    client.setBufferSize(512);
}

void MqttDeviceService::
    reloadConfiguration()
{
    client.setServer(
        config.mqttHost().c_str(),
        EcoSenseConfig::MQTT_PORT
    );
}

void MqttDeviceService::tick()
{
    if (!wifi.isConnected()) {
        return;
    }

    if (!client.connected()) {
        tryConnect();
        return;
    }

    const uint32_t started =
        micros();

    client.loop();

    const uint32_t duration =
        micros() - started;

    performance.recordTiming(
        "mqtt_loop",
        duration
    );

    performance.addBusyMicros(
        duration
    );
}

void MqttDeviceService::tryConnect()
{
    const uint32_t now =
        millis();

    if (
        now - lastReconnectAttempt <
        EcoSenseConfig::
            MQTT_RECONNECT_INTERVAL_MS
    ) {
        return;
    }

    lastReconnectAttempt = now;

    String clientId =
        "EcoSense-ESP32-";

    clientId += String(
        static_cast<uint32_t>(
            ESP.getEfuseMac()
        ),
        HEX
    );

    if (
        client.connect(
            clientId.c_str()
        )
    ) {
        client.subscribe(
            EcoSenseConfig::
                COMMAND_FAN_TOPIC
        );

        client.subscribe(
            EcoSenseConfig::
                COMMAND_EXHAUST_TOPIC
        );

        logs.info(
            "MQTT conectado e topicos de comando assinados"
        );

        return;
    }

    logs.warning(
        "Falha MQTT, estado="
        + String(client.state())
    );
}

bool MqttDeviceService::
    publishTelemetry(
        const SensorSample& sample
    )
{
    if (!client.connected()) {
        return false;
    }

    const uint32_t started =
        micros();

    StaticJsonDocument<256> document;

    document["device_id"] =
        EcoSenseConfig::DEVICE_ID;

    document["epoch"] =
        static_cast<long>(
            sample.epoch
        );

    document["uptime_ms"] =
        sample.uptimeMs;

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

    char payload[256];

    const size_t length =
        serializeJson(
            document,
            payload,
            sizeof(payload)
        );

    const bool published =
        client.publish(
            EcoSenseConfig::
                TELEMETRY_TOPIC,
            reinterpret_cast<
                const uint8_t*
            >(payload),
            length,
            false
        );

    const uint32_t duration =
        micros() - started;

    performance.recordTiming(
        "mqtt_publish",
        duration
    );

    performance.addBusyMicros(
        duration
    );

    return published;
}

bool MqttDeviceService::
    isConnected() const
{
    return client.connected();
}

void MqttDeviceService::callback(
    char* topic,
    byte* payload,
    unsigned int length
)
{
    if (!instance) {
        return;
    }

    String message;
    message.reserve(length);

    for (
        unsigned int i = 0;
        i < length;
        i++
    ) {
        message +=
            static_cast<char>(
                payload[i]
            );
    }

    instance->handleMessage(
        String(topic),
        message
    );
}

void MqttDeviceService::handleMessage(
    const String& topic,
    const String& payload
)
{
    const uint32_t started =
        micros();

    StaticJsonDocument<256> document;

    const DeserializationError error =
        deserializeJson(
            document,
            payload
        );

    if (error) {
        logs.warning(
            "Comando MQTT com JSON invalido"
        );

        return;
    }

    const long commandId =
        document["command_id"] |
        0L;

    const String command =
        document["command"] |
        "";

    String actuator;

    if (
        topic ==
        EcoSenseConfig::
            COMMAND_FAN_TOPIC
    ) {
        actuator = "fan";
    } else if (
        topic ==
        EcoSenseConfig::
            COMMAND_EXHAUST_TOPIC
    ) {
        actuator = "exhaust";
    } else {
        return;
    }

    String executionError;

    const bool success =
        actuators.execute(
            actuator,
            command,
            executionError
        );

    if (success) {
        logs.info(
            "Atuador "
            + actuator
            + " executou "
            + command
        );
    } else {
        logs.warning(
            "Falha no atuador "
            + actuator
            + ": "
            + executionError
        );
    }

    publishAck(
        commandId,
        actuator,
        success,
        success
            ? "executed"
            : executionError
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
}

void MqttDeviceService::publishAck(
    long commandId,
    const String& actuator,
    bool success,
    const String& message
)
{
    if (!client.connected()) {
        return;
    }

    StaticJsonDocument<256> document;

    document["command_id"] =
        commandId;

    document["device_id"] =
        EcoSenseConfig::DEVICE_ID;

    document["actuator"] =
        actuator;

    document["status"] =
        success
            ? "executed"
            : "error";

    document["message"] =
        message;

    char payload[256];

    const size_t length =
        serializeJson(
            document,
            payload,
            sizeof(payload)
        );

    client.publish(
        EcoSenseConfig::
            COMMAND_ACK_TOPIC,
        reinterpret_cast<
            const uint8_t*
        >(payload),
        length,
        false
    );
}
