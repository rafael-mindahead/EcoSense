#include "SensorService.h"

#include <Wire.h>

#include "AppConfig.h"

namespace {
constexpr uint8_t ENS160_ADDRESS_PRIMARY = 0x53;
constexpr uint8_t ENS160_ADDRESS_SECONDARY = 0x52;

constexpr uint8_t ENS160_REG_OPMODE = 0x10;
constexpr uint8_t ENS160_REG_DATA_AQI = 0x21;

constexpr uint8_t ENS160_OPMODE_STANDARD = 0x02;
}

SensorService::SensorService(
    TimeService& time
) :
    time(time),
    dht(
        EcoSenseConfig::DHT_PIN,
        EcoSenseConfig::DHT_TYPE
    )
{
}

void SensorService::begin()
{
    dht.begin();

    pinMode(
        EcoSenseConfig::LDR_PIN,
        INPUT
    );

    Wire.begin(
        EcoSenseConfig::ENS160_SDA_PIN,
        EcoSenseConfig::ENS160_SCL_PIN
    );

    ens160Available =
        detectEns160();

    if (ens160Available) {
        writeEns160Register(
            ENS160_REG_OPMODE,
            ENS160_OPMODE_STANDARD
        );
    }
}

SensorSample SensorService::read()
{
    SensorSample sample;

    sample.epoch =
        time.epoch();

    sample.uptimeMs =
        millis();

    sample.temperature =
        dht.readTemperature();

    sample.humidity =
        dht.readHumidity();

    sample.dhtValid =
        !isnan(
            sample.temperature
        ) &&
        !isnan(
            sample.humidity
        );

    sample.luminosity =
        analogRead(
            EcoSenseConfig::LDR_PIN
        );

    uint8_t aqi = 0;

    sample.ens160Valid =
        ens160Available &&
        readEns160Register(
            ENS160_REG_DATA_AQI,
            aqi
        );

    sample.airQuality =
        sample.ens160Valid
            ? static_cast<int>(aqi & 0x07)
            : -1;

    portENTER_CRITICAL(
        &sampleMux
    );

    lastSample = sample;

    portEXIT_CRITICAL(
        &sampleMux
    );

    return sample;
}

SensorSample SensorService::latest() const
{
    SensorSample sample;

    portENTER_CRITICAL(
        &sampleMux
    );

    sample = lastSample;

    portEXIT_CRITICAL(
        &sampleMux
    );

    return sample;
}

bool SensorService::detectEns160()
{
    const uint8_t addresses[] = {
        ENS160_ADDRESS_PRIMARY,
        ENS160_ADDRESS_SECONDARY
    };

    for (const uint8_t address : addresses) {
        Wire.beginTransmission(
            address
        );

        if (
            Wire.endTransmission() ==
            0
        ) {
            ens160Address =
                address;

            return true;
        }
    }

    ens160Address = 0;

    return false;
}

bool SensorService::writeEns160Register(
    uint8_t reg,
    uint8_t value
)
{
    if (ens160Address == 0) {
        return false;
    }

    Wire.beginTransmission(
        ens160Address
    );

    Wire.write(reg);
    Wire.write(value);

    return
        Wire.endTransmission() ==
        0;
}

bool SensorService::readEns160Register(
    uint8_t reg,
    uint8_t& value
)
{
    if (ens160Address == 0) {
        return false;
    }

    Wire.beginTransmission(
        ens160Address
    );

    Wire.write(reg);

    if (
        Wire.endTransmission(false) !=
        0
    ) {
        return false;
    }

    if (
        Wire.requestFrom(
            static_cast<int>(
                ens160Address
            ),
            1
        ) != 1
    ) {
        return false;
    }

    value = Wire.read();

    return true;
}
