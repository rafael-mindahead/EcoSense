# EcoSense

EcoSense é um sistema ciberfísico de monitoramento ambiental baseado em ESP32. Para o TDE de Performance em Sistemas Ciberfísicos, o **ESP32 é o servidor principal e autônomo**: ele lê sensores, controla atuadores, persiste histórico e logs, mede desempenho, gerencia energia e serve a interface Web diretamente ao navegador.

PHP, PostgreSQL, Docker e MQTT permanecem no repositório como **integração externa opcional**. O núcleo local do TDE continua funcionando mesmo sem notebook, Docker, PHP, banco de dados ou broker MQTT.

## Arquitetura principal do TDE

```text
DHT22 / LDR / qualidade do ar
             ↓
        ESP32 + FreeRTOS
        ├── Web Server
        ├── API local
        ├── Interface Web embarcada
        ├── Performance
        ├── Histórico 24h em LittleFS
        ├── Logs em LittleFS
        ├── Configuração em NVS
        ├── OTA
        ├── Light Sleep
        └── 2x D4184
             ├── Cooler ventilador 12 V
             └── Cooler exaustor 12 V

Integração opcional:
ESP32 → MQTT → PHP → PostgreSQL
```

## Interface Web

A interface oficial está em `web/` e é incorporada ao firmware durante o build por `esp32/scripts/embed_web.py`. Ela é servida pelo próprio ESP32 e não depende de CDN ou internet para os gráficos.

Páginas disponíveis:

- Dashboard;
- Histórico de 24h;
- Performance;
- Logs;
- Alertas derivados de WARNING/ERROR locais;
- Dispositivos;
- Controle de ventilador e exaustor;
- Configurações;
- Sobre;
- Changelog.

A página **Performance** concentra CPU estimada, frequência da CPU, heap, stack, flash, PSRAM quando disponível, quantidade de tasks, tempo de execução das funções, Wi-Fi/AP/RSSI, dados dos sensores, histórico de 24h e OTA.

## Firmware

O firmware fica em `esp32/` e usa PlatformIO + Arduino C++.

Entradas preparadas:

- DHT22 / AM2302 em GPIO 4;
- LDR analógico em GPIO 34;
- entrada analógica de qualidade do ar em GPIO 35 enquanto o ENS160 + AHT21 não é integrado.

Saídas:

- GPIO 26 → D4184 → ventilador 12 V;
- GPIO 27 → D4184 → exaustor 12 V.

Os relés **não fazem parte da arquitetura final**. O ESP32 envia apenas o sinal lógico aos D4184; a potência dos coolers vem da fonte externa de 12 V.

O ENS160 + AHT21 está planejado como sensor adicional de qualidade do ar e ainda precisa ser integrado ao firmware via I²C.

## FreeRTOS e desempenho

O EcoSense cria quatro tasks próprias:

- `SensorTask`;
- `NetworkTask`;
- `WebTask`;
- `MaintenanceTask`.

O código da aplicação não usa `delay()`; usa `vTaskDelay()` para ceder CPU.

A interrupção de hardware de validação utiliza o GPIO 0.

## Persistência e observabilidade

- histórico local em LittleFS com consulta das últimas 24 horas;
- logs INFO, WARNING e ERROR persistidos em flash;
- rotação automática para os últimos 1000 registros;
- exportação dos logs pela interface;
- configurações de Wi-Fi, intervalo de sensores e MQTT persistidas em NVS/Preferences.

## Energia

A interface permite acionar **Light Sleep** por um intervalo configurável.

## Configuração local

Na raiz do repositório:

```bash
cp esp32/include/secrets.example.h esp32/include/secrets.h
```

Edite `esp32/include/secrets.h` com a rede que será usada. MQTT pode ser preenchido ou deixado apenas para a integração externa.

## Compilar

```bash
pio run -d esp32
```

O build gera automaticamente `esp32/include/GeneratedWebAssets.h` a partir de `web/`.

## Gravar no ESP32

```bash
pio run -d esp32 --target upload
pio device monitor
```

Abra no navegador o IP mostrado no monitor serial. Se a rede configurada não estiver disponível, o dispositivo pode iniciar o AP de fallback `EcoSense-Setup`.

## API local principal

| Método | Endpoint | Função |
|---|---|---|
| GET | `/api/status` | Estado do ESP32, Wi-Fi/AP e atuadores |
| GET | `/api/sensors/latest` | Última leitura |
| GET | `/api/sensors/history` | Histórico local de 24h |
| GET | `/api/performance` | Métricas de desempenho |
| GET | `/api/config` | Configuração atual |
| POST | `/api/config/sample` | Intervalo de leitura |
| POST | `/api/config/wifi` | Troca de Wi-Fi |
| POST | `/api/config/mqtt` | Broker MQTT opcional |
| POST | `/api/actuator` | Fan/exhaust |
| GET | `/api/logs` | Logs recentes |
| GET | `/api/logs/export` | Exportação dos logs |
| POST | `/api/energy/light-sleep` | Light Sleep |
| POST | `/api/ota` | Atualização OTA |

## Integração externa opcional

As pastas `api/`, `database/`, `docker/` e a infraestrutura MQTT/PostgreSQL permanecem como extensão do projeto para histórico de longo prazo, regras adicionais e integrações. Elas **não são dependência da demonstração local do TDE**.

## Testes

Validação estrutural do firmware:

```bash
bash tests/esp32/requirements.sh
```

Build:

```bash
pio run -d esp32
```

## Pendências físicas antes da entrega

O software não substitui a validação física exigida pelo TDE. Ainda é necessário:

- testar DHT22 e LDR individualmente;
- integrar/testar ENS160 + AHT21 se ele permanecer no escopo;
- testar os dois coolers via D4184;
- validar fonte e montagem elétrica;
- medir/analisar consumo de energia;
- acondicionar tudo em caixa física;
- preencher nomes completos e e-mails na página Sobre;
- coletar fotos, screenshots e evidências dos testes.

A matriz detalhada está em `Docs/ESP_SERVER_REQUIREMENTS.md` e o roteiro de apresentação em `Docs/DEMO_CHECKLIST.md`.

---

**EcoSense — Monitorar. Entender. Agir.**
