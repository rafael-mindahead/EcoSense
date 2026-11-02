#include <Arduino.h>

#include "ActuatorHardwareService.h"
#include "AppConfig.h"
#include "ConfigService.h"
#include "EnergyService.h"
#include "HistoryService.h"
#include "LogService.h"
#include "MqttDeviceService.h"
#include "OtaService.h"
#include "PerformanceService.h"
#include "SensorService.h"
#include "TimeService.h"
#include "WebServerService.h"
#include "WifiService.h"

namespace {

ConfigService configService;
WifiService wifiService(
    configService
);

TimeService timeService(
    wifiService
);

LogService logService(
    timeService
);

HistoryService historyService(
    timeService
);

SensorService sensorService(
    timeService
);

ActuatorHardwareService
    actuatorService;

PerformanceService
    performanceService(
        wifiService
    );

MqttDeviceService mqttService(
    configService,
    wifiService,
    actuatorService,
    logService,
    performanceService
);

EnergyService energyService(
    wifiService,
    logService
);

OtaService otaService(
    logService
);

WebServerService webService(
    configService,
    wifiService,
    timeService,
    sensorService,
    historyService,
    logService,
    performanceService,
    actuatorService,
    mqttService,
    energyService,
    otaService
);

TaskHandle_t sensorTaskHandle =
    nullptr;

TaskHandle_t networkTaskHandle =
    nullptr;

TaskHandle_t webTaskHandle =
    nullptr;

TaskHandle_t maintenanceTaskHandle =
    nullptr;

volatile uint32_t interruptCount =
    0;

portMUX_TYPE interruptMux =
    portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR hardwareInterrupt()
{
    portENTER_CRITICAL_ISR(
        &interruptMux
    );

    interruptCount++;

    portEXIT_CRITICAL_ISR(
        &interruptMux
    );
}

void sensorTask(void*)
{
    for (;;) {
        const uint32_t sensorStarted =
            micros();

        const SensorSample sample =
            sensorService.read();

        const uint32_t sensorDuration =
            micros() -
            sensorStarted;

        performanceService
            .recordTiming(
                "sensor_read",
                sensorDuration
            );

        performanceService
            .addBusyMicros(
                sensorDuration
            );

        const uint32_t historyStarted =
            micros();

        historyService.append(
            sample
        );

        const uint32_t historyDuration =
            micros() -
            historyStarted;

        performanceService
            .recordTiming(
                "history_write",
                historyDuration
            );

        performanceService
            .addBusyMicros(
                historyDuration
            );

        if (!sample.dhtValid) {
            logService.warning(
                "DHT22 sem leitura valida; verifique o sensor fisico"
            );
        }

        vTaskDelay(
            pdMS_TO_TICKS(
                configService
                    .sensorIntervalMs()
            )
        );
    }
}

void networkTask(void*)
{
    uint32_t lastPublishedSample =
        0;

    for (;;) {
        const uint32_t started =
            micros();

        wifiService.tick();
        timeService.tick();
        mqttService.tick();

        const SensorSample sample =
            sensorService.latest();

        if (
            sample.uptimeMs != 0 &&
            sample.uptimeMs !=
                lastPublishedSample &&
            mqttService.isConnected()
        ) {
            if (
                mqttService
                    .publishTelemetry(
                        sample
                    )
            ) {
                lastPublishedSample =
                    sample.uptimeMs;
            }
        }

        performanceService
            .addBusyMicros(
                micros() -
                started
            );

        vTaskDelay(
            pdMS_TO_TICKS(10)
        );
    }
}

void webTask(void*)
{
    for (;;) {
        webService.tick();

        vTaskDelay(
            pdMS_TO_TICKS(5)
        );
    }
}

void maintenanceTask(void*)
{
    uint32_t handledInterrupts =
        0;

    for (;;) {
        energyService.tick();

        uint32_t currentInterrupts;

        portENTER_CRITICAL(
            &interruptMux
        );

        currentInterrupts =
            interruptCount;

        portEXIT_CRITICAL(
            &interruptMux
        );

        if (
            currentInterrupts !=
            handledInterrupts
        ) {
            handledInterrupts =
                currentInterrupts;

            logService.info(
                "Interrupcao de hardware detectada. Total="
                + String(
                    handledInterrupts
                )
            );
        }

        vTaskDelay(
            pdMS_TO_TICKS(50)
        );
    }
}

}

void setup()
{
    Serial.begin(115200);

    configService.begin();

    wifiService.begin();

    logService.begin();
    historyService.begin();

    sensorService.begin();
    actuatorService.begin();

    performanceService.begin();

    mqttService.begin();

    webService.begin();

    pinMode(
        EcoSenseConfig::
            INTERRUPT_PIN,
        INPUT_PULLUP
    );

    attachInterrupt(
        digitalPinToInterrupt(
            EcoSenseConfig::
                INTERRUPT_PIN
        ),
        hardwareInterrupt,
        FALLING
    );

    logService.info(
        "EcoSense ESP32 Server iniciado"
    );

    xTaskCreatePinnedToCore(
        sensorTask,
        "SensorTask",
        4096,
        nullptr,
        1,
        &sensorTaskHandle,
        1
    );

    xTaskCreatePinnedToCore(
        networkTask,
        "NetworkTask",
        6144,
        nullptr,
        2,
        &networkTaskHandle,
        0
    );

    xTaskCreatePinnedToCore(
        webTask,
        "WebTask",
        8192,
        nullptr,
        2,
        &webTaskHandle,
        1
    );

    xTaskCreatePinnedToCore(
        maintenanceTask,
        "MaintenanceTask",
        4096,
        nullptr,
        1,
        &maintenanceTaskHandle,
        0
    );

    performanceService
        .registerTask(
            "SensorTask",
            sensorTaskHandle
        );

    performanceService
        .registerTask(
            "NetworkTask",
            networkTaskHandle
        );

    performanceService
        .registerTask(
            "WebTask",
            webTaskHandle
        );

    performanceService
        .registerTask(
            "MaintenanceTask",
            maintenanceTaskHandle
        );

    Serial.println();
    Serial.println(
        "EcoSense operando como servidor autonomo."
    );

    Serial.println(
        "Acesse o IP exibido pela rede ou o AP EcoSense-Setup."
    );
}

void loop()
{
    vTaskDelay(
        pdMS_TO_TICKS(1000)
    );
}
