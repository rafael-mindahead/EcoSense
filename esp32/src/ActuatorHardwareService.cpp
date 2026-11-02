#include "ActuatorHardwareService.h"

#include "AppConfig.h"

void ActuatorHardwareService::begin()
{
    pinMode(
        EcoSenseConfig::
            FAN_DRIVER_PIN,
        OUTPUT
    );

    pinMode(
        EcoSenseConfig::
            EXHAUST_DRIVER_PIN,
        OUTPUT
    );

    writeDriver(
        EcoSenseConfig::
            FAN_DRIVER_PIN,
        false
    );

    writeDriver(
        EcoSenseConfig::
            EXHAUST_DRIVER_PIN,
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

        writeDriver(
            EcoSenseConfig::
                FAN_DRIVER_PIN,
            on
        );

        return true;
    }

    if (
        normalizedActuator ==
        "exhaust"
    ) {
        exhaustState = on;

        writeDriver(
            EcoSenseConfig::
                EXHAUST_DRIVER_PIN,
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

void ActuatorHardwareService::writeDriver(
    uint8_t pin,
    bool on
)
{
    const uint8_t level =
        (
            on ==
            EcoSenseConfig::
                ACTUATOR_DRIVER_ACTIVE_HIGH
        )
            ? HIGH
            : LOW;

    digitalWrite(
        pin,
        level
    );
}
