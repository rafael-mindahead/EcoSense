# EcoSense ESP32

Firmware do ESP32 responsável por conectar o dispositivo ao Wi-Fi e publicar telemetria no broker MQTT do EcoSense.

## Configuração local

1. Copie `include/secrets.example.h` para `include/secrets.h`.
2. Preencha `WIFI_SSID` e `WIFI_PASSWORD`.
3. Preencha `MQTT_HOST` com o IP local da máquina que executa o Mosquitto.
4. Compile e envie o firmware com PlatformIO.

Exemplo:

```cpp
#define WIFI_SSID "MinhaRede"
#define WIFI_PASSWORD "minha-senha"
#define MQTT_HOST "172.20.10.2"
```

Para descobrir o IP do Mac na interface Wi-Fi:

```bash
ipconfig getifaddr en0
```

Comandos PlatformIO:

```bash
pio run
pio run --target upload
pio device monitor
```

Se o comando `pio` não estiver no PATH:

```bash
~/.platformio/penv/bin/pio run
~/.platformio/penv/bin/pio run --target upload
~/.platformio/penv/bin/pio device monitor
```

O arquivo `include/secrets.h` contém credenciais e configuração local de rede e não deve ser versionado.
