# EcoSense ESP32

Firmware do ESP32 responsável por conectar o dispositivo ao Wi-Fi e publicar telemetria no broker MQTT do EcoSense.

## Configuração local

1. Copie `include/secrets.example.h` para `include/secrets.h`.
2. Preencha o SSID e a senha da rede Wi-Fi.
3. Ajuste `MQTT_HOST` em `src/main.cpp` para o IP local da máquina que executa o Mosquitto.
4. Compile e envie o firmware com PlatformIO.

```bash
pio run
pio run --target upload
pio device monitor
```

O arquivo `include/secrets.h` contém credenciais locais e não deve ser versionado.
