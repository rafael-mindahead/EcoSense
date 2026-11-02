# Matriz de aderência — TDE Performance de Sistemas Ciberfísicos

Esta matriz acompanha a reforma em que o ESP32 passa a ser o servidor principal do EcoSense.

## Implementado nesta branch

| Requisito | Implementação |
|---|---|
| Interface Web embarcada | `WebServerService` serve a interface diretamente do ESP32 |
| Página de Performance | CPU estimada, heap, stack, flash, PSRAM, timings, tasks e Wi-Fi |
| 5+ funções medidas | sensor, histórico, MQTT loop/publish, atuador e Web |
| Sensores + histórico 24h | `SensorService` + `HistoryService` em LittleFS |
| Estado Wi-Fi/AP + RSSI | `WifiService` + endpoint de performance |
| OTA | upload de firmware pela interface Web |
| Logs INFO/WARNING/ERROR | `LogService` persistente em LittleFS |
| Últimos 1000 logs | rotação automática em flash |
| Exportação de logs | `/api/logs/export` |
| Frequência de sensores configurável | Preferences/NVS via Web |
| Parâmetros operacionais | intervalo, MQTT, Wi-Fi, energia e atuadores |
| Troca de Wi-Fi sem regravar | `/api/config/wifi` + NVS |
| Guia Sobre | incluído na interface; dados pessoais ainda precisam ser preenchidos |
| Changelog | incluído na interface embarcada |
| Persistência | histórico e logs locais em flash |
| Sem `delay()` no código da aplicação | validação automatizada em `tests/esp32/requirements.sh` |
| Mais de uma task | 4 tasks próprias do EcoSense |
| Interrupção de hardware | ISR em GPIO 0 |
| Gerenciamento de energia | Light Sleep acionável pela Web |
| C/C++ | firmware PlatformIO/Arduino C++ |
| GitHub público | repositório EcoSense |
| Dois sensores mínimos | DHT22 + LDR; ENS160 + AHT21 planejado como sensor adicional |
| Dois acionamentos previstos | fan + exhaust, ambos coolers 12 V comandados por drivers MOSFET D4184 |
| MQTT opcional | integração externa sem ser dependência do servidor local |

## Hardware definido para a montagem

- ESP32 DevKit WROOM-32;
- DHT22 / AM2302;
- módulo LDR analógico;
- ENS160 + AHT21 planejado para qualidade do ar;
- 2x D4184 compatíveis com controle lógico de 3,3 V;
- 2x coolers 12 V;
- fonte externa 12 V / 2 A;
- conector P4 fêmea com borne;
- protoboards e jumpers;
- caixa física na etapa final.

Os relés inicialmente previstos não fazem mais parte da arquitetura física. O ESP32 fornece apenas o sinal lógico aos D4184; a potência dos dois coolers vem da fonte externa de 12 V.

## Exige validação física antes da entrega

Os itens abaixo não podem ser considerados concluídos apenas por código:

- sensores conectados e medindo valores reais;
- ventilador e exaustor conectados aos dois D4184 e alimentados pela fonte externa;
- integração do ENS160 + AHT21 caso seja mantido no escopo final;
- montagem elétrica segura;
- caixa física;
- testes individuais dos sensores;
- testes individuais dos atuadores;
- medição e análise real de consumo de energia;
- evidências fotográficas e de execução.

## Exige conteúdo da equipe

A página Sobre possui a estrutura exigida, mas ainda precisa receber:

- nome completo de todos os integrantes;
- e-mails de contato.

## Evidências sugeridas para a defesa

1. ESP inicia sem backend externo.
2. Navegador abre diretamente o IP do ESP.
3. Página Performance atualiza métricas.
4. Alteração do intervalo de leitura sem recompilar.
5. Alteração de Wi-Fi pela interface.
6. Exportação dos logs persistidos.
7. Upload OTA.
8. Acionamento independente dos dois coolers via D4184.
9. Histórico local de 24h.
10. Pressão do botão de interrupção aparecendo no log.
11. Light Sleep acionado pela Web.
12. MQTT/backend desligados enquanto a interface local continua funcionando.
