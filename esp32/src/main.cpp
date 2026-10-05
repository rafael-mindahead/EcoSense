#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include "secrets.h"

namespace {
constexpr char MQTT_HOST[] = "192.168.1.18";
constexpr uint16_t MQTT_PORT = 1883;
constexpr char TELEMETRY_TOPIC[] = "ecosense/device/1/telemetry";
constexpr unsigned long TELEMETRY_INTERVAL_MS = 5000;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

unsigned long lastTelemetry = 0;

void connectWifi()
{
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    Serial.print("Conectando ao Wi-Fi");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi conectado!");

    Serial.print("IP do ESP32: ");
    Serial.println(WiFi.localIP());
}

void connectMqtt()
{
    while (!mqttClient.connected()) {
        Serial.print("Conectando ao MQTT...");

        String clientId = "EcoSense-ESP32-";
        clientId += String(static_cast<uint32_t>(ESP.getEfuseMac()), HEX);

        if (mqttClient.connect(clientId.c_str())) {
            Serial.println(" conectado!");
            return;
        }

        Serial.print(" falhou. Estado: ");
        Serial.println(mqttClient.state());

        delay(2000);
    }
}

void publishTelemetry()
{
    const float temperature =
        random(220, 320) / 10.0f;

    const float humidity =
        random(400, 800) / 10.0f;

    const int luminosity =
        random(300, 900);

    const int airQuality =
        random(70, 100);

    char payload[180];

    snprintf(
        payload,
        sizeof(payload),
        "{\"temperature\":%.1f,\"humidity\":%.1f,\"luminosity\":%d,\"air_quality\":%d}",
        temperature,
        humidity,
        luminosity,
        airQuality
    );

    const bool published =
        mqttClient.publish(
            TELEMETRY_TOPIC,
            payload
        );

    Serial.print("Telemetria: ");
    Serial.println(payload);

    Serial.println(
        published
            ? "Publicada com sucesso!"
            : "Falha ao publicar."
    );
}
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("EcoSense ESP32 iniciado");

    randomSeed(esp_random());

    connectWifi();

    mqttClient.setServer(
        MQTT_HOST,
        MQTT_PORT
    );
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }

    if (!mqttClient.connected()) {
        connectMqtt();
    }

    mqttClient.loop();

    const unsigned long now = millis();

    if (
        now - lastTelemetry >=
        TELEMETRY_INTERVAL_MS
    ) {
        lastTelemetry = now;
        publishTelemetry();
    }
}
