#!/bin/bash

set -e

BASE_URL="http://localhost:8080"

echo "→ Testando listagem de devices..."

STATUS=$(curl -s \
    -o /tmp/ecosense_devices.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/devices")

if [ "$STATUS" != "200" ]; then
    echo "❌ Esperado 200, recebido $STATUS"
    cat /tmp/ecosense_devices.json
    exit 1
fi

echo "✅ Listagem de devices funcionando"

echo "→ Testando device individual..."

STATUS=$(curl -s \
    -o /tmp/ecosense_device.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/devices/show?id=1")

if [ "$STATUS" != "200" ]; then
    echo "❌ Esperado 200, recebido $STATUS"
    cat /tmp/ecosense_device.json
    exit 1
fi

echo "✅ Consulta individual funcionando"

echo "→ Testando id inválido..."

STATUS=$(curl -s \
    -o /tmp/ecosense_device_invalid.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/devices/show?id=-1")

if [ "$STATUS" != "422" ]; then
    echo "❌ Esperado 422, recebido $STATUS"
    cat /tmp/ecosense_device_invalid.json
    exit 1
fi

echo "✅ Validação de device funcionando"

echo "→ Testando device inexistente..."

STATUS=$(curl -s \
    -o /tmp/ecosense_device_not_found.json \
    -w "%{http_code}" \
    "$BASE_URL/api/v1/devices/show?id=999999")

if [ "$STATUS" != "404" ]; then
    echo "❌ Esperado 404, recebido $STATUS"
    cat /tmp/ecosense_device_not_found.json
    exit 1
fi

echo "✅ Device inexistente retorna 404"