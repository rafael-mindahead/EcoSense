#pragma once

#include <Arduino.h>

namespace EcoSenseConfig {

constexpr uint8_t DEVICE_ID = 1;

constexpr uint16_t HTTP_PORT = 80;
constexpr uint16_t MQTT_PORT = 1883;

constexpr uint8_t DHT_PIN = 4;
constexpr uint8_t DHT_TYPE = 22;
constexpr uint8_t LDR_PIN = 34;
constexpr uint8_t AIR_QUALITY_PIN = 35;

constexpr uint8_t FAN_DRIVER_PIN = 26;
constexpr uint8_t EXHAUST_DRIVER_PIN = 27;
constexpr bool ACTUATOR_DRIVER_ACTIVE_HIGH = true;

constexpr uint8_t INTERRUPT_PIN = 0;

constexpr uint32_t DEFAULT_SENSOR_INTERVAL_MS = 30000;
constexpr uint32_t MIN_SENSOR_INTERVAL_MS = 5000;
constexpr uint32_t MAX_SENSOR_INTERVAL_MS = 3600000;

constexpr uint32_t MQTT_RECONNECT_INTERVAL_MS = 5000;
constexpr uint32_t WIFI_RECONNECT_INTERVAL_MS = 10000;
constexpr uint32_t WIFI_AP_FALLBACK_MS = 15000;

constexpr size_t MAX_LOG_ENTRIES = 1000;
constexpr size_t MAX_HISTORY_FILE_BYTES = 900000;

constexpr char TELEMETRY_TOPIC[] =
    "ecosense/device/1/telemetry";

constexpr char COMMAND_FAN_TOPIC[] =
    "ecosense/device/1/commands/fan";

constexpr char COMMAND_EXHAUST_TOPIC[] =
    "ecosense/device/1/commands/exhaust";

constexpr char COMMAND_ACK_TOPIC[] =
    "ecosense/device/1/commands/ack";

constexpr char SETUP_AP_SSID[] =
    "EcoSense-Setup";

constexpr char PROJECT_VERSION[] =
    "2.0.0-esp-server";

}
