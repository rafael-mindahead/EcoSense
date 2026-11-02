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
  "esp32/scripts/embed_web.py"
  "web/html/dashboard.html"
  "web/html/historico.html"
  "web/html/performance.html"
  "web/html/logs.html"
  "web/html/configuracoes.html"
  "web/html/sobre.html"
  "web/html/changelog.html"
  "web/js/api.js"
)

for file in "${required_files[@]}"; do
  if [ ! -f "$file" ]; then
    echo "❌ Arquivo obrigatório ausente: $file"
    exit 1
  fi
done

echo "✅ Serviços e páginas obrigatórias presentes"

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
grep -q "GeneratedWebAssets.h" esp32/src/WebServerService.cpp
grep -q "extra_scripts = pre:scripts/embed_web.py" esp32/platformio.ini

echo "✅ Performance, logs, Wi-Fi, energia, OTA e front embarcado configurados"

echo "→ Validando autonomia do front..."

if grep -R -nE 'localhost:8080|cdn\.jsdelivr\.net|/api/v1/' web --include='*.html' --include='*.js'; then
  echo "❌ Front ainda depende da arquitetura externa ou de CDN"
  exit 1
fi

grep -q "/api/sensors/latest" web/js/api.js
grep -q "/api/performance" web/js/api.js
grep -q "/api/config/wifi" web/js/api.js
grep -q "/api/energy/light-sleep" web/js/api.js

echo "✅ Front fala diretamente com o ESP32 e não depende de internet"
