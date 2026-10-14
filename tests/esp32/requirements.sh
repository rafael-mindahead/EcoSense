#!/bin/bash

set -e

echo "→ Validando estrutura do servidor embarcado..."

required_files=(
  "esp32/src/main.cpp"
  "esp32/include/WebServerService.h"
  "esp32/include/PerformanceService.h"
  "esp32/include/LogService.h"
  "esp32/include/HistoryService.h"
  "esp32/include/EnergyService.h"
  "esp32/include/OtaService.h"
  "esp32/include/MqttDeviceService.h"
  "esp32/include/SensorService.h"
  "esp32/include/ActuatorHardwareService.h"
)

for file in "${required_files[@]}"; do
  if [ ! -f "$file" ]; then
    echo "❌ Arquivo obrigatório ausente: $file"
    exit 1
  fi
done

echo "✅ Serviços embarcados presentes"

echo "→ Procurando delay() bloqueante..."

if grep -R -nE '(^|[^A-Za-z_])delay[[:space:]]*\(' esp32/src --include='*.cpp' --include='*.h'; then
  echo "❌ delay() bloqueante encontrado no firmware"
  exit 1
fi

echo "✅ Nenhum delay() bloqueante"

echo "→ Validando FreeRTOS e interrupção..."

grep -q "xTaskCreatePinnedToCore" esp32/src/main.cpp
grep -q "attachInterrupt" esp32/src/main.cpp

echo "✅ Multitarefa e interrupção configuradas"

echo "→ Validando recursos obrigatórios..."

grep -q "/api/performance" esp32/src/WebServerService.cpp
grep -q "/api/logs/export" esp32/src/WebServerService.cpp
grep -q "/api/config/wifi" esp32/src/WebServerService.cpp
grep -q "/api/energy/light-sleep" esp32/src/WebServerService.cpp
grep -q "/api/ota" esp32/src/WebServerService.cpp

echo "✅ Performance, logs, Wi-Fi, energia e OTA expostos pelo ESP32"
