#include "EnergyService.h"

#include <WiFi.h>
#include <esp_sleep.h>

EnergyService::EnergyService(
    WifiService& wifi,
    LogService& logs
) :
    wifi(wifi),
    logs(logs)
{
}

bool EnergyService::requestLightSleep(
    uint32_t seconds
)
{
    if (
        seconds < 1 ||
        seconds > 3600 ||
        pending
    ) {
        return false;
    }

    sleepSeconds = seconds;

    executeAtMs =
        millis() + 500;

    pending = true;

    return true;
}

void EnergyService::tick()
{
    if (!pending) {
        return;
    }

    if (
        static_cast<int32_t>(
            millis() -
            executeAtMs
        ) < 0
    ) {
        return;
    }

    pending = false;

    logs.info(
        "Entrando em Light Sleep por "
        + String(sleepSeconds)
        + " segundos"
    );

    WiFi.disconnect(
        true,
        false
    );

    WiFi.mode(WIFI_OFF);

    esp_sleep_enable_timer_wakeup(
        static_cast<uint64_t>(
            sleepSeconds
        ) *
        1000000ULL
    );

    esp_light_sleep_start();

    logs.info(
        "ESP32 acordou do Light Sleep"
    );

    wifi.begin();
}

bool EnergyService::isPending() const
{
    return pending;
}
