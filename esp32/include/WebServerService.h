#pragma once

#include <Arduino.h>
#include <WebServer.h>

#include "ActuatorHardwareService.h"
#include "ConfigService.h"
#include "EnergyService.h"
#include "HistoryService.h"
#include "LogService.h"
#include "MqttDeviceService.h"
#include "OtaService.h"
#include "PerformanceService.h"
#include "SensorService.h"
#include "TimeService.h"
#include "WifiService.h"

class WebServerService
{
public:
    WebServerService(
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
    );

    void begin();
    void tick();

private:
    ConfigService& config;
    WifiService& wifi;
    TimeService& time;
    SensorService& sensors;
    HistoryService& history;
    LogService& logs;
    PerformanceService& performance;
    ActuatorHardwareService& actuators;
    MqttDeviceService& mqtt;
    EnergyService& energy;
    OtaService& ota;

    WebServer server;

    bool restartRequested = false;
    uint32_t restartAtMs = 0;

    void registerRoutes();

    void sendJson(
        int status,
        const String& json
    );

    void sendError(
        int status,
        const String& message
    );

    String latestSensorJson() const;
    String statusJson() const;
    String configJson() const;
};
