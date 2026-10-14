#include "ActuatorHardwareService.h"

#include "AppConfig.h"

void ActuatorHardwareService::begin()
{
    pinMode(
        EcoSenseConfig::
            FAN_RELAY_PIN,
        OUTPUT
    );

    pinMode(
        EcoSenseConfig::
            EXHAUST_RELAY_PIN,
        OUTPUT
    );

    writeRelay(
        EcoSenseConfig::
            FAN_RELAY_PIN,
        false
    );

    writeRelay(
        EcoSenseConfig::
            EXHAUST_RELAY_PIN,
        false
    );
}

bool ActuatorHardwareService::execute(
    const String& actuator,
    const String& command,
    String& error
)
{
    const String normalizedActuator =
        actuator;

    String normalizedCommand =
        command;

    normalizedCommand.toUpperCase();

    if (
        normalizedCommand != "ON" &&
        normalizedCommand != "OFF"
    ) {
        error =
            "command must be ON or OFF";

        return false;
    }

    const bool on =
        normalizedCommand == "ON";

    if (
        normalizedActuator ==
        "fan"
    ) {
        fanState = on;

        writeRelay(
            EcoSenseConfig::
                FAN_RELAY_PIN,
            on
        );

        return true;
    }

    if (
        normalizedActuator ==
        "exhaust"
    ) {
        exhaustState = on;

        writeRelay(
            EcoSenseConfig::
                EXHAUST_RELAY_PIN,
            on
        );

        return true;
    }

    error =
        "invalid actuator";

    return false;
}

bool ActuatorHardwareService::fanOn() const
{
    return fanState;
}

bool ActuatorHardwareService::exhaustOn() const
{
    return exhaustState;
}

void ActuatorHardwareService::writeRelay(
    uint8_t pin,
    bool on
)
{
    const uint8_t level =
        (
            on ==
            EcoSenseConfig::
                RELAY_ACTIVE_HIGH
        )
            ? HIGH
            : LOW;

    digitalWrite(
        pin,
        level
    );
}
