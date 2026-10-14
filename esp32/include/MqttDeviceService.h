#pragma once

#include <Arduino.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#include "ActuatorHardwareService.h"
#include "ConfigService.h"
#include "LogService.h"
#include "PerformanceService.h"
#include "SensorService.h"
#include "WifiService.h"

class MqttDeviceService
{
public:
    MqttDeviceService(
        ConfigService& config,
        WifiService& wifi,
        ActuatorHardwareService& actuators,
        LogService& logs,
        PerformanceService& performance
    );

    void begin();
    void tick();

    bool publishTelemetry(
        const SensorSample& sample
    );

    bool isConnected();

    void reloadConfiguration();

private:
    ConfigService& config;
    WifiService& wifi;
    ActuatorHardwareService& actuators;
    LogService& logs;
    PerformanceService& performance;

    WiFiClient wifiClient;
    PubSubClient client;

    uint32_t lastReconnectAttempt = 0;

    static MqttDeviceService* instance;

    static void callback(
        char* topic,
        byte* payload,
        unsigned int length
    );

    void handleMessage(
        const String& topic,
        const String& payload
    );

    void tryConnect();

    void publishAck(
        long commandId,
        const String& actuator,
        bool success,
        const String& message
    );
};
