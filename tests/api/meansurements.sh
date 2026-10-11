#!/bin/bash

set -e

BASE_URL="http://localhost:8080"
TODAY=$(date +%Y-%m-%d)

echo "→ Testando medição válida..."

STATUS=$(curl -s \
    -o /tmp/ecosense_measurement.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/meansurements" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "temperature": 25,
        "humidity": 60,
        "luminosity": 700,
        "air_quality": 90
    }')

if [ "$STATUS" != "201" ]; then
    echo "❌ Esperado 201, recebido $STATUS"
    cat /tmp/ecosense_measurement.json
    exit 1
fi

echo "✅ Medição válida aceita"

echo "→ Testando validação centralizada..."

STATUS=$(curl -s \
    -o /tmp/ecosense_invalid.json \
    -w "%{http_code}" \
    -X POST \
    "$BASE_URL/api/v1/meansurements" \
    -H "Content-Type: application/json" \
    -d '{
        "device_id": 1,
        "temperature": 25,
        "humidity": 900,
        "luminosity": -50,
        "air_quality": -10
    }')

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_invalid.json
    exit 1
fi

echo "✅ Validação rejeitou valores inválidos"

echo "→ Testando latest por device..."

STATUS=$(curl -s \
    -o /tmp/ecosense_latest.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/meansurements/latest?device_id=1")

if [ "$STATUS" != "200" ]; then
    echo "❌ Latest esperava 200, recebeu $STATUS"
    cat /tmp/ecosense_latest.json
    exit 1
fi

echo "✅ Latest por device funcionando"

echo "→ Testando histórico filtrado..."

STATUS=$(curl -s \
    -o /tmp/ecosense_history.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/meansurements?device_id=1&limit=5")

if [ "$STATUS" != "200" ]; then
    echo "❌ Histórico esperava 200, recebeu $STATUS"
    cat /tmp/ecosense_history.json
    exit 1
fi

if ! grep -q '"device_id":1' /tmp/ecosense_history.json; then
    echo "❌ Histórico não retornou o device esperado"
    cat /tmp/ecosense_history.json
    exit 1
fi

echo "✅ Histórico por device funcionando"

echo "→ Testando filtro por período..."

STATUS=$(curl -s \
    -o /tmp/ecosense_history_date.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/meansurements?device_id=1&from=$TODAY&to=$TODAY&limit=20")

if [ "$STATUS" != "200" ]; then
    echo "❌ Filtro por período esperava 200, recebeu $STATUS"
    cat /tmp/ecosense_history_date.json
    exit 1
fi

echo "✅ Filtro por período funcionando"

echo "→ Testando device_id inválido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_bad_device.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/meansurements?device_id=-2")

if [ "$STATUS" != "422" ]; then
    echo "❌ device_id inválido esperava 422, recebeu $STATUS"
    cat /tmp/ecosense_bad_device.json
    exit 1
fi

echo "✅ device_id inválido rejeitado"

echo "→ Testando intervalo invertido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_bad_range.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/meansurements?from=2099-12-01&to=2000-01-01")

if [ "$STATUS" != "422" ]; then
    echo "❌ Intervalo invertido esperava 422, recebeu $STATUS"
    cat /tmp/ecosense_bad_range.json
    exit 1
fi

echo "✅ Intervalo invertido rejeitado"
