#include "WifiService.h"

#include "AppConfig.h"
#include "secrets.h"

WifiService::WifiService(
    ConfigService& config
) :
    config(config)
{
}

void WifiService::begin()
{
    WiFi.mode(WIFI_AP_STA);

    startedAt = millis();

    startStation();
}

void WifiService::startStation()
{
    const String configuredSsid =
        config.wifiSsid();

    if (configuredSsid.isEmpty()) {
        startAccessPoint();
        return;
    }

    WiFi.begin(
        configuredSsid.c_str(),
        config.wifiPassword().c_str()
    );

    lastReconnectAttempt =
        millis();
}

void WifiService::startAccessPoint()
{
    if (accessPointActive) {
        return;
    }

    WiFi.softAP(
        EcoSenseConfig::SETUP_AP_SSID,
        ECOSENSE_AP_PASSWORD
    );

    accessPointActive = true;
}

void WifiService::tick()
{
    const uint32_t now =
        millis();

    if (
        WiFi.status() ==
        WL_CONNECTED
    ) {
        return;
    }

    if (
        !accessPointActive &&
        now - startedAt >=
            EcoSenseConfig::
                WIFI_AP_FALLBACK_MS
    ) {
        startAccessPoint();
    }

    if (
        now - lastReconnectAttempt <
        EcoSenseConfig::
            WIFI_RECONNECT_INTERVAL_MS
    ) {
        return;
    }

    lastReconnectAttempt = now;

    WiFi.reconnect();
}

bool WifiService::isConnected() const
{
    return
        WiFi.status() ==
        WL_CONNECTED;
}

bool WifiService::isAccessPointActive() const
{
    return accessPointActive;
}

int32_t WifiService::rssi() const
{
    if (!isConnected()) {
        return 0;
    }

    return WiFi.RSSI();
}

String WifiService::stationIp() const
{
    if (!isConnected()) {
        return "";
    }

    return
        WiFi.localIP()
            .toString();
}

String WifiService::accessPointIp() const
{
    if (!accessPointActive) {
        return "";
    }

    return
        WiFi.softAPIP()
            .toString();
}

String WifiService::ssid() const
{
    if (!isConnected()) {
        return
            config.wifiSsid();
    }

    return WiFi.SSID();
}

void WifiService::reconfigure(
    const String& ssid,
    const String& password
)
{
    config.saveWifi(
        ssid,
        password
    );

    WiFi.disconnect(
        false,
        false
    );

    startedAt =
        millis();

    WiFi.begin(
        ssid.c_str(),
        password.c_str()
    );

    lastReconnectAttempt =
        millis();
}
