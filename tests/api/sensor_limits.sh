#!/bin/bash

set -e

BASE_URL="http://localhost:8080"

echo "→ Testando listagem de limites..."

STATUS=$(curl -s \
    -o /tmp/ecosense_limits.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/sensor-limits")

if [ "$STATUS" != "200" ]; then
    echo "❌ Esperado 200, recebido $STATUS"
    cat /tmp/ecosense_limits.json
    exit 1
fi

echo "✅ Listagem de limites funcionando"

echo "→ Testando device_id inválido nos limites..."

STATUS=$(curl -s \
    -o /tmp/ecosense_limit_bad_device.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/sensor-limits?device_id=-1")

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_limit_bad_device.json
    exit 1
fi

echo "✅ device_id inválido rejeitado"

echo "→ Testando métrica inválida..."

STATUS=$(curl -s \
    -o /tmp/ecosense_limit_bad_metric.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/sensor-limits" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "metric": "invalid_metric",
        "min_value": 10,
        "max_value": 20
    }')

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_limit_bad_metric.json
    exit 1
fi

echo "✅ Métrica inválida rejeitada"

echo "→ Testando intervalo inválido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_limit_bad_range.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/sensor-limits" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "metric": "temperature",
        "min_value": 50,
        "max_value": 10
    }')

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_limit_bad_range.json
    exit 1
fi

echo "✅ Intervalo inválido rejeitado"
