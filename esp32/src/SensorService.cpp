#include "SensorService.h"

#include "AppConfig.h"

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

    pinMode(
        EcoSenseConfig::
            AIR_QUALITY_PIN,
        INPUT
    );
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

    sample.airQuality =
        analogRead(
            EcoSenseConfig::
                AIR_QUALITY_PIN
        );

    lastSample = sample;

    return sample;
}

SensorSample SensorService::latest() const
{
    return lastSample;
}
