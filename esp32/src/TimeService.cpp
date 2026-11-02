#include "TimeService.h"

#include <time.h>

TimeService::TimeService(
    WifiService& wifi
) :
    wifi(wifi)
{
}

void TimeService::tick()
{
    if (
        configured ||
        !wifi.isConnected()
    ) {
        return;
    }

    configTime(
        -3 * 3600,
        0,
        "pool.ntp.org",
        "time.google.com"
    );

    configured = true;
}

bool TimeService::isSynchronized() const
{
    return
        time(nullptr) >
        1700000000;
}

time_t TimeService::epoch() const
{
    if (!isSynchronized()) {
        return 0;
    }

    return time(nullptr);
}

String TimeService::timestamp() const
{
    const time_t now =
        epoch();

    if (now == 0) {
        return
            "uptime:"
            + String(millis());
    }

    struct tm timeInfo {};

    localtime_r(
        &now,
        &timeInfo
    );

    char buffer[24];

    strftime(
        buffer,
        sizeof(buffer),
        "%Y-%m-%d %H:%M:%S",
        &timeInfo
    );

    return String(buffer);
}
