#!/bin/bash

set -e

echo
echo "=============================="
echo " EcoSense Automated Tests"
echo "=============================="
echo

bash tests/api/health.sh
bash tests/api/meansurements.sh
bash tests/api/devices.sh
bash tests/mqtt/telemetry.sh

echo
echo "=============================="
echo " ✅ ALL TESTS PASSED"
echo "=============================="