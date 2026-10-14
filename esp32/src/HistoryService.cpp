#include "HistoryService.h"

#include <LittleFS.h>
#include <ArduinoJson.h>

#include "AppConfig.h"
#include "SensorService.h"

namespace {
constexpr char HISTORY_FILE[] =
    "/history.csv";

constexpr char HISTORY_TEMP_FILE[] =
    "/history.tmp";
}

HistoryService::HistoryService(
    TimeService& time
) :
    time(time)
{
}

bool HistoryService::begin()
{
    return LittleFS.begin(true);
}

void HistoryService::append(
    const SensorSample& sample
)
{
    File file =
        LittleFS.open(
            HISTORY_FILE,
            FILE_APPEND
        );

    if (!file) {
        return;
    }

    file.print(
        static_cast<long>(
            sample.epoch
        )
    );
    file.print(",");

    file.print(
        sample.temperature,
        2
    );
    file.print(",");

    file.print(
        sample.humidity,
        2
    );
    file.print(",");

    file.print(
        sample.luminosity
    );
    file.print(",");

    file.print(
        sample.airQuality
    );
    file.print(",");

    file.print(
        sample.dhtValid
            ? 1
            : 0
    );

    file.println();

    file.close();

    trimIfNeeded();
}

void HistoryService::trimIfNeeded()
{
    File file =
        LittleFS.open(
            HISTORY_FILE,
            FILE_READ
        );

    if (!file) {
        return;
    }

    const size_t size =
        file.size();

    file.close();

    if (
        size <=
        EcoSenseConfig::
            MAX_HISTORY_FILE_BYTES
    ) {
        return;
    }

    File source =
        LittleFS.open(
            HISTORY_FILE,
            FILE_READ
        );

    File target =
        LittleFS.open(
            HISTORY_TEMP_FILE,
            FILE_WRITE
        );

    if (
        !source ||
        !target
    ) {
        if (source) {
            source.close();
        }

        if (target) {
            target.close();
        }

        return;
    }

    source.seek(size / 4);

    source.readStringUntil('\n');

    while (source.available()) {
        target.write(
            source.read()
        );
    }

    source.close();
    target.close();

    LittleFS.remove(
        HISTORY_FILE
    );

    LittleFS.rename(
        HISTORY_TEMP_FILE,
        HISTORY_FILE
    );
}

String HistoryService::
    readLast24HoursJson(
        size_t maxPoints
    ) const
{
    File file =
        LittleFS.open(
            HISTORY_FILE,
            FILE_READ
        );

    if (!file) {
        return "[]";
    }

    const time_t now =
        time.epoch();

    const time_t threshold =
        now > 0
            ? now - 86400
            : 0;

    size_t eligible = 0;

    while (file.available()) {
        const String line =
            file.readStringUntil('\n');

        const int comma =
            line.indexOf(',');

        if (comma <= 0) {
            continue;
        }

        const time_t epoch =
            static_cast<time_t>(
                line.substring(
                    0,
                    comma
                ).toInt()
            );

        if (
            threshold == 0 ||
            epoch >= threshold
        ) {
            eligible++;
        }
    }

    file.seek(0);

    const size_t step =
        eligible > maxPoints
            ? max(
                static_cast<size_t>(1),
                eligible / maxPoints
            )
            : 1;

    DynamicJsonDocument document(
        maxPoints * 96 + 1024
    );

    JsonArray array =
        document.to<JsonArray>();

    size_t eligibleIndex = 0;

    while (
        file.available() &&
        array.size() < maxPoints
    ) {
        const String line =
            file.readStringUntil('\n');

        int positions[5];
        int from = 0;
        bool valid = true;

        for (int i = 0; i < 5; i++) {
            positions[i] =
                line.indexOf(
                    ',',
                    from
                );

            if (positions[i] < 0) {
                valid = false;
                break;
            }

            from =
                positions[i] + 1;
        }

        if (!valid) {
            continue;
        }

        const time_t epoch =
            static_cast<time_t>(
                line.substring(
                    0,
                    positions[0]
                ).toInt()
            );

        if (
            threshold != 0 &&
            epoch < threshold
        ) {
            continue;
        }

        if (
            eligibleIndex++ %
                step !=
            0
        ) {
            continue;
        }

        JsonObject item =
            array.add<JsonObject>();

        item["epoch"] =
            static_cast<long>(epoch);

        item["temperature"] =
            line.substring(
                positions[0] + 1,
                positions[1]
            ).toFloat();

        item["humidity"] =
            line.substring(
                positions[1] + 1,
                positions[2]
            ).toFloat();

        item["luminosity"] =
            line.substring(
                positions[2] + 1,
                positions[3]
            ).toInt();

        item["air_quality"] =
            line.substring(
                positions[3] + 1,
                positions[4]
            ).toInt();

        item["dht_valid"] =
            line.substring(
                positions[4] + 1
            ).toInt() == 1;
    }

    file.close();

    String json;
    serializeJson(
        document,
        json
    );

    return json;
}

const char* HistoryService::filePath() const
{
    return HISTORY_FILE;
}
