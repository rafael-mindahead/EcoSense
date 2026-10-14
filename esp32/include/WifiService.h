#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "ConfigService.h"

class WifiService
{
public:
    explicit WifiService(
        ConfigService& config
    );

    void begin();
    void tick();

    bool isConnected() const;
    bool isAccessPointActive() const;

    int32_t rssi() const;
    String stationIp() const;
    String accessPointIp() const;
    String ssid() const;

    void reconfigure(
        const String& ssid,
        const String& password
    );

private:
    ConfigService& config;

    uint32_t startedAt = 0;
    uint32_t lastReconnectAttempt = 0;
    bool accessPointActive = false;

    bool reconfigurePending = false;
    uint32_t reconfigureAtMs = 0;

    void startStation();
    void startAccessPoint();
};
