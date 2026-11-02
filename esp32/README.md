# EcoSense ESP32 Server

Nesta branch o ESP32 deixa de ser apenas um cliente de telemetria e passa a operar como **servidor autônomo do EcoSense**.

O núcleo do projeto funciona mesmo sem PHP, PostgreSQL ou dashboard externo:

```text
Sensores
   ↓
ESP32 + FreeRTOS
   ├── Web Server / REST API
   ├── Performance
   ├── Logs em LittleFS
   ├── Histórico local de 24h
   ├── Configuração Wi-Fi
   ├── OTA
   ├── Light Sleep
   ├── Atuadores DC via drivers MOSFET
   └── MQTT opcional para integração externa
```

## Hardware preparado

Entradas:

- DHT22 em GPIO 4 — temperatura e umidade;
- LDR analógico em GPIO 34 — luminosidade;
- a implementação atual mantém uma entrada analógica de qualidade do ar em GPIO 35;
- para a montagem final está planejado ENS160 + AHT21 via I2C, que ainda será integrado ao firmware.

Saídas:

- driver MOSFET D4184 do ventilador em GPIO 26;
- driver MOSFET D4184 do exaustor em GPIO 27;
- dois coolers de 12 V alimentados por fonte externa 12 V / 2 A.

Os 12 V dos atuadores não passam pelos GPIOs do ESP32. Os GPIOs 26 e 27 fornecem apenas o sinal lógico de controle aos D4184.

Interrupção:

- GPIO 0 (botão BOOT) é utilizado como interrupção de hardware durante a validação acadêmica.

> Os pinos podem ser alterados em `include/AppConfig.h` conforme a montagem física final.

## Configuração local

Copie:

```bash
cp include/secrets.example.h include/secrets.h
```

Preencha:

```cpp
#define WIFI_SSID "SUA_REDE"
#define WIFI_PASSWORD "SUA_SENHA"
#define MQTT_HOST "IP_DO_BROKER"
#define ECOSENSE_AP_PASSWORD "SENHA_DO_AP"
```

`secrets.h` não deve ser versionado.

Se `ECOSENSE_AP_PASSWORD` não estiver definido em um arquivo local antigo, o firmware utiliza um valor de desenvolvimento como fallback. Troque-o antes da entrega.

## Compilar e gravar

```bash
~/.platformio/penv/bin/pio run
~/.platformio/penv/bin/pio run --target upload
~/.platformio/penv/bin/pio device monitor
```

## Acesso Web

Com o ESP conectado ao Wi-Fi, abra no navegador o IP exibido no monitor serial.

Se ele não conseguir conectar à rede configurada, após alguns segundos ele disponibiliza um ponto de acesso:

```text
EcoSense-Setup
```

A interface embarcada inclui:

- Dashboard;
- Performance;
- Histórico de 24h;
- Configuração;
- Logs;
- Sobre;
- Changelog;
- OTA.

## Performance

A página Performance apresenta:

- estimativa de uso de CPU pelas tasks EcoSense;
- frequência da CPU;
- heap total, livre e mínimo;
- flash;
- PSRAM quando disponível;
- número de tasks;
- stack high-water mark das tasks criadas pelo EcoSense;
- RSSI e estado da conexão Wi-Fi/AP;
- tempo de execução de pelo menos cinco operações:
  - leitura de sensores;
  - persistência de histórico;
  - loop MQTT;
  - publicação MQTT;
  - execução de atuador;
  - atendimento Web.

## FreeRTOS

O firmware cria tasks separadas para:

- `SensorTask`;
- `NetworkTask`;
- `WebTask`;
- `MaintenanceTask`.

O código da aplicação não utiliza `delay()`; as tasks cedem CPU com `vTaskDelay()`.

## Logs

Os logs de INFO, WARNING e ERROR são persistidos em LittleFS, limitados aos 1000 registros mais recentes e podem ser exportados pela interface Web.

## Histórico

As amostras são gravadas localmente em LittleFS e o endpoint Web fornece histórico das últimas 24 horas com downsampling para visualização.

## Wi-Fi sem regravação

SSID e senha podem ser alterados pela página Configuração. Os dados ficam em NVS/Preferences e o ESP tenta conectar à nova rede sem recompilar o firmware.

## OTA

A página OTA recebe o `firmware.bin` gerado pelo PlatformIO e atualiza o firmware pelo navegador.

## Energia

A página Configuração permite solicitar Light Sleep por um intervalo configurável.

## MQTT

MQTT deixa de ser dependência obrigatória do núcleo local. Quando disponível, replica telemetria e recebe comandos externos para fan/exhaust, respondendo com ACK.

## Pendências físicas

O código está preparado para DHT22, LDR, drivers D4184, interrupção e servidor embarcado. A conformidade final do TDE ainda depende da montagem real:

- conectar e validar os sensores reais;
- integrar o ENS160 + AHT21 ao firmware se ele for mantido no escopo final;
- conectar dois coolers 12 V aos drivers D4184;
- validar a fonte externa de 12 V e a montagem elétrica;
- acondicionar o sistema em caixa apropriada;
- preencher nomes e e-mails da equipe na página Sobre;
- coletar evidências de testes, performance e consumo de energia.
