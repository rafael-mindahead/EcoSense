#include "ConfigService.h"

#include "AppConfig.h"
#include "secrets.h"

void ConfigService::begin()
{
    preferences.begin(
        "ecosense",
        false
    );
}

String ConfigService::wifiSsid() const
{
    return preferences.getString(
        "wifi_ssid",
        WIFI_SSID
    );
}

String ConfigService::wifiPassword() const
{
    return preferences.getString(
        "wifi_pass",
        WIFI_PASSWORD
    );
}

String ConfigService::mqttHost() const
{
    return preferences.getString(
        "mqtt_host",
        MQTT_HOST
    );
}

uint32_t ConfigService::sensorIntervalMs() const
{
    const uint32_t value =
        preferences.getUInt(
            "sensor_ms",
            EcoSenseConfig::
                DEFAULT_SENSOR_INTERVAL_MS
        );

    return constrain(
        value,
        EcoSenseConfig::
            MIN_SENSOR_INTERVAL_MS,
        EcoSenseConfig::
            MAX_SENSOR_INTERVAL_MS
    );
}

void ConfigService::saveWifi(
    const String& ssid,
    const String& password
)
{
    preferences.putString(
        "wifi_ssid",
        ssid
    );

    preferences.putString(
        "wifi_pass",
        password
    );
}

void ConfigService::saveMqttHost(
    const String& host
)
{
    preferences.putString(
        "mqtt_host",
        host
    );
}

void ConfigService::saveSensorInterval(
    uint32_t intervalMs
)
{
    intervalMs = constrain(
        intervalMs,
        EcoSenseConfig::
            MIN_SENSOR_INTERVAL_MS,
        EcoSenseConfig::
            MAX_SENSOR_INTERVAL_MS
    );

    preferences.putUInt(
        "sensor_ms",
        intervalMs
    );
}
