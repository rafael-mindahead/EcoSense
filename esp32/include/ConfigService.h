#pragma once

#include <Arduino.h>
#include <Preferences.h>

class ConfigService
{
public:
    void begin();

    String wifiSsid() const;
    String wifiPassword() const;
    String mqttHost() const;
    uint32_t sensorIntervalMs() const;

    void saveWifi(
        const String& ssid,
        const String& password
    );

    void saveMqttHost(
        const String& host
    );

    void saveSensorInterval(
        uint32_t intervalMs
    );

private:
    mutable Preferences preferences;
};
