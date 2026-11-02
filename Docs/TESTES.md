# Testes — EcoSense

Este documento é o roteiro oficial para validar o EcoSense no ESP32 físico antes da entrega do TDE de **Performance em Sistemas Ciberfísicos**.

A arquitetura principal do TDE é autônoma: **ESP32 + sensores + atuadores + interface Web embarcada**. Docker, PHP, PostgreSQL e Mosquitto/MQTT são integrações opcionais e **não precisam estar rodando para os testes obrigatórios abaixo**.

> **Importante:** faça os testes na ordem deste documento. Primeiro compile e valide o ESP32 sozinho; depois conecte e teste cada sensor/atuador individualmente; por último faça os testes de integração.

---

## 1. O que precisa estar disponível

### Hardware atual

- 1x ESP32 WROOM-32 / ESP32 DevKit;
- 1x DHT22 / AM2302 — temperatura e umidade;
- 1x LDR — luminosidade;
- 2x módulos MOSFET D4184;
- 2x coolers 12 V;
- 1x fonte externa 12 V / 2 A;
- protoboard, jumpers e conexões da montagem.

### Sensor de qualidade do ar

O **ENS160 + AHT21** está planejado para a montagem final, porém **ainda não está integrado ao firmware atual**.

Não conecte o ENS160 ao GPIO 35. O ENS160 usa **I²C**, enquanto o firmware ainda mantém a entrada antiga de qualidade do ar como leitura analógica no GPIO 35.

Para os testes atuais do requisito mínimo de sensores, **DHT22 + LDR já fornecem dois sensores físicos de entrada**.

### Pinos usados pelo firmware atual

| Componente | Pino |
|---|---:|
| DHT22 | GPIO 4 |
| LDR | GPIO 34 |
| Qualidade do ar analógica antiga | GPIO 35 |
| D4184 / ventilador | GPIO 26 |
| D4184 / exaustor | GPIO 27 |
| Interrupção de hardware | GPIO 0 |

---

## 2. Segurança elétrica antes de energizar

Antes de ligar o sistema:

- os coolers de 12 V devem receber energia da **fonte externa de 12 V**, não do ESP32;
- GPIO 26 e GPIO 27 devem fornecer apenas o **sinal lógico** aos módulos D4184;
- nunca aplique 12 V em um GPIO do ESP32;
- confira a polaridade dos coolers e da fonte;
- confira se a montagem possui o GND de referência conforme o esquema usado;
- evite conduzir a corrente dos coolers pelos trilhos finos da protoboard;
- faça o primeiro teste de cada cooler individualmente antes de ligar os dois ao mesmo tempo.

Se houver aquecimento anormal, cheiro, ruído elétrico ou reinicializações do ESP32, desligue a alimentação e revise a montagem.

---

## 3. Baixar a versão correta

Use a branch `main`.

```bash
git clone https://github.com/rafael-mindahead/EcoSense.git
cd EcoSense
git switch main
git pull origin main
```

Para confirmar:

```bash
git status
git log -1 --oneline
```

A árvore deve estar limpa antes dos testes.

---

## 4. Preparar a configuração local

Crie o arquivo local de configuração:

```bash
cp esp32/include/secrets.example.h esp32/include/secrets.h
```

Abra:

```text
esp32/include/secrets.h
```

Preencha pelo menos:

```cpp
#define WIFI_SSID "NOME_DA_REDE"
#define WIFI_PASSWORD "SENHA_DA_REDE"
```

O `MQTT_HOST` pode permanecer sem uso durante os testes obrigatórios do TDE, porque MQTT/Docker são opcionais para a arquitetura local.

A senha padrão do AP de fallback é:

```text
ecosense123
```

---

## 5. Validar o código antes de gravar

Na raiz do repositório:

```bash
bash tests/esp32/requirements.sh
```

Resultado esperado: o script deve terminar sem erros.

Esse teste verifica, entre outros pontos:

- ausência de `delay()` bloqueante;
- múltiplas tasks FreeRTOS;
- interrupção de hardware;
- endpoints obrigatórios;
- interface Web embarcada;
- ausência de dependência de `localhost:8080`, `/api/v1` e CDN;
- pelo menos cinco funções monitoradas para desempenho;
- configuração da partição OTA.

---

## 6. Compilar o firmware

```bash
pio run -d esp32
```

Resultado esperado:

```text
SUCCESS
```

O build também incorpora automaticamente os arquivos da pasta `web/` no firmware.

Se `pio` não existir no terminal, instale/configure o PlatformIO antes de continuar.

---

## 7. Conectar o ESP32 e gravar

Confira se a placa foi detectada:

```bash
pio device list
```

Depois grave:

```bash
pio run -d esp32 --target upload
```

Resultado esperado: upload concluído sem erro.

Abra o monitor serial:

```bash
pio device monitor
```

Velocidade configurada no projeto:

```text
115200 baud
```

### Evidência a salvar

Tire um print do terminal mostrando:

- boot do ESP32;
- endereço IP;
- inicialização dos serviços;
- ausência de erro crítico.

---

## 8. Testar acesso à interface Web

No monitor serial, identifique o IP do ESP32.

Abra no navegador:

```text
http://IP_DO_ESP32/
```

Resultado esperado: carregar a interface oficial do EcoSense.

Se o ESP32 não conseguir entrar no Wi-Fi configurado, verifique se foi criado o AP de fallback:

```text
EcoSense-Setup
```

Conecte-se a ele e use o IP de AP informado pelo dispositivo.

### Páginas que devem abrir

- Dashboard;
- Histórico;
- Performance;
- Logs;
- Alertas;
- Dispositivos;
- Controle;
- Configurações;
- Sobre;
- Changelog.

Nenhuma dessas páginas deve depender de Docker, PHP ou conexão com a internet.

---

# 9. Testes isolados dos sensores

O professor exige teste individual dos sensores antes da integração completa.

## 9.1 DHT22

Conecte apenas o DHT22 conforme a montagem planejada.

Pino de dados:

```text
GPIO 4
```

Verifique no Dashboard ou em:

```text
/api/sensors/latest
```

### Resultado esperado

- temperatura com valor plausível;
- umidade com valor plausível;
- `dht_valid` indicando leitura válida;
- valores atualizando conforme o intervalo configurado.

### Teste prático

1. deixe o sensor estabilizar;
2. anote temperatura e umidade;
3. aproxime a mão do sensor sem encostar;
4. observe se há variação coerente;
5. salve print da interface.

Se aparecer `N/A` continuamente, conferir alimentação, GPIO 4 e conexão do sensor.

---

## 9.2 LDR

Entrada atual:

```text
GPIO 34
```

### Teste prático

1. registre a leitura em iluminação normal;
2. cubra o LDR;
3. registre a nova leitura;
4. ilumine o LDR com uma fonte de luz;
5. registre novamente.

### Resultado esperado

A leitura ADC deve variar de forma clara entre escuro e iluminado.

Não é necessário que o valor seja expresso em lux; o teste atual é da leitura analógica e da sua variação.

---

## 9.3 ENS160 + AHT21

**Não marcar este teste como concluído ainda.**

O ENS160 + AHT21 será integrado via I²C em uma etapa posterior. O firmware atual ainda não possui a implementação final desse módulo.

---

# 10. Testes isolados dos atuadores

Os atuadores obrigatórios do EcoSense são:

- cooler #1 — ventilador;
- cooler #2 — exaustor.

Cada um é comandado por um D4184.

## 10.1 Ventilador

Fluxo:

```text
GPIO 26 → D4184 #1 → Cooler 12 V #1
```

Na interface, acesse **Controle**.

Teste:

1. clique em Ligar ventilador;
2. confirme que somente o cooler correspondente gira;
3. clique em Desligar;
4. confirme parada completa;
5. repita pelo menos três vezes.

Resultado esperado:

- resposta consistente;
- nenhum reset do ESP32;
- módulo sem aquecimento anormal.

---

## 10.2 Exaustor

Fluxo:

```text
GPIO 27 → D4184 #2 → Cooler 12 V #2
```

Repita o mesmo procedimento:

1. ligar;
2. verificar o cooler correto;
3. desligar;
4. repetir pelo menos três vezes.

---

## 10.3 Teste dos dois atuadores simultaneamente

Depois dos testes individuais:

1. ligue ventilador;
2. ligue exaustor;
3. mantenha ambos ligados por alguns minutos;
4. desligue um de cada vez;
5. confira se o ESP32 continua acessível pela Web.

Resultado esperado: os dois atuadores funcionam de forma independente e simultânea.

---

# 11. Teste de integração completa

Com DHT22, LDR e os dois coolers conectados:

1. iniciar o ESP32;
2. abrir o Dashboard;
3. confirmar leitura dos sensores;
4. abrir Controle;
5. ligar/desligar cada cooler;
6. retornar ao Dashboard;
7. confirmar que os sensores continuam atualizando;
8. abrir Histórico;
9. conferir se novas amostras são adicionadas;
10. abrir Performance e verificar as métricas em tempo real.

Resultado esperado: sensores, atuadores, Web Server, persistência e tasks funcionam ao mesmo tempo sem travamento.

---

# 12. Teste da página Performance

Abrir:

```text
/performance
```

Verificar obrigatoriamente:

- uso estimado de CPU;
- frequência da CPU;
- heap total/livre;
- flash;
- PSRAM quando aplicável;
- número de tasks;
- stack das tasks;
- Wi-Fi conectado;
- SSID;
- RSSI;
- estado do AP;
- IP da estação/AP;
- último valor dos sensores;
- histórico/gráfico;
- área OTA.

## Cinco funções monitoradas

Confirmar na tabela de timings a presença de pelo menos:

- `sensor_read`;
- `history_write`;
- `wifi_tick`;
- `time_tick`;
- `web_handle`.

Tire screenshots dessa página para usar como evidência no relatório.

---

# 13. Teste dos logs

Abra **Logs**.

Verifique:

- registros INFO;
- registros WARNING;
- registros ERROR quando ocorrerem;
- data/hora dos eventos;
- persistência em flash.

Use o botão de exportação.

Também pode acessar:

```text
/api/logs
/api/logs/export
```

Resultado esperado: os logs permanecem disponíveis mesmo sem PHP/PostgreSQL.

---

# 14. Teste de configuração dinâmica

Abra **Configurações**.

## 14.1 Intervalo dos sensores

1. anote o intervalo atual;
2. altere para outro valor válido;
3. salve;
4. observe a frequência das novas leituras;
5. reinicie o ESP32;
6. confirme que a configuração persistiu.

## 14.2 Wi-Fi

Faça este teste somente quando puder reconectar ao dispositivo.

1. altere o SSID/senha pela interface;
2. aplique;
3. aguarde a reconexão;
4. identifique o novo IP;
5. reabra a interface.

É normal o navegador perder a conexão no momento da troca de rede.

Resultado esperado: troca de Wi-Fi sem regravar firmware.

---

# 15. Teste de persistência

O TDE exige histórico mínimo de 24 horas.

Para comprovar corretamente:

1. deixe o EcoSense em operação contínua;
2. mantenha as leituras sendo gravadas;
3. registre horário de início;
4. após pelo menos 24 horas, abra Histórico;
5. confirme dados antigos e recentes;
6. reinicie o ESP32 durante o período e confirme que os dados persistem;
7. salve screenshots com data/hora.

Também confirme que os logs continuam disponíveis após reinicialização.

---

# 16. Teste de interrupção de hardware

A implementação atual utiliza:

```text
GPIO 0
```

Durante a execução normal do firmware, acione o botão/pino associado à interrupção conforme a montagem.

Resultado esperado: o evento de interrupção deve ser processado sem bloquear as outras tasks.

**Não mantenha GPIO 0 acionado durante reset/boot**, pois esse pino também participa do modo de gravação do ESP32.

Registre a evidência no monitor serial/logs.

---

# 17. Teste de Light Sleep

Abra **Configurações** e use a área de gerenciamento de energia.

Sugestão inicial:

```text
10 segundos
```

1. acione Light Sleep;
2. observe a perda temporária de comunicação;
3. aguarde o tempo configurado;
4. confirme retorno do ESP32;
5. reabra a interface;
6. confira se configurações e histórico continuam disponíveis.

---

# 18. Teste de consumo de energia

O professor exige avaliação de desempenho e consumo.

Se houver multímetro, wattímetro USB ou medidor apropriado:

### Cenário A — operação normal

- sensores ativos;
- Web ativa;
- coolers desligados;
- registrar consumo.

### Cenário B — atuadores ativos

- ligar os dois coolers;
- registrar consumo total da fonte/atuadores;
- observar estabilidade do ESP32.

### Cenário C — Light Sleep

- acionar Light Sleep;
- registrar consumo durante o período;
- comparar com operação normal.

Registre equipamento usado, tensão, corrente/potência medida e duração do teste.

---

# 19. Teste OTA

Primeiro gere um firmware:

```bash
pio run -d esp32
```

Arquivo esperado:

```text
esp32/.pio/build/esp32dev/firmware.bin
```

Na página **Performance**, selecione esse `firmware.bin` na área OTA e faça o upload.

Resultado esperado:

1. upload concluído;
2. ESP32 reinicia;
3. dispositivo volta à rede;
4. interface volta a responder;
5. histórico/configurações continuam consistentes.

Faça este teste somente depois de confirmar que o upload via cabo USB já funciona.

---

# 20. Docker, PHP, PostgreSQL e MQTT

## Para o TDE principal

**Não precisa subir Docker.**

O ESP32 deve funcionar sozinho para:

- sensores;
- atuadores;
- interface Web;
- Dashboard;
- Performance;
- Histórico;
- Logs;
- configuração;
- troca de Wi-Fi;
- OTA;
- Light Sleep.

## Teste opcional da arquitetura externa

Somente se quiser validar a extensão do projeto:

```bash
docker compose up -d --build
docker compose ps
```

Essa etapa testa MQTT/PHP/PostgreSQL, mas **não deve ser necessária para demonstrar o funcionamento principal do EcoSense**.

---

# 21. Evidências que precisam ser coletadas

Para cada teste, guardar:

- foto da montagem;
- print da interface;
- print do monitor serial quando relevante;
- resultado esperado;
- resultado obtido;
- data/hora;
- observações ou falhas.

Priorizar evidências de:

- DHT22 funcionando;
- LDR reagindo à luz;
- ventilador ligado/desligado;
- exaustor ligado/desligado;
- dois coolers simultâneos;
- Dashboard;
- Histórico;
- Performance;
- cinco timings;
- Logs e exportação;
- alteração de intervalo;
- troca de Wi-Fi;
- Light Sleep;
- OTA;
- persistência após reinício;
- histórico após 24 horas;
- consumo de energia;
- montagem completa.

---

# 22. Como reportar uma falha para o grupo

Se algum teste falhar, não alterar várias partes do projeto ao mesmo tempo.

Enviar:

```text
TESTE:
Ex.: DHT22

ETAPA:
Ex.: leitura no Dashboard

ESPERADO:
Ex.: temperatura e umidade válidas

OBTIDO:
Ex.: N/A

MONITOR SERIAL:
colar as linhas do erro

FOTO/PRINT:
anexar

ALTERAÇÕES FEITAS:
informar qualquer fio, configuração ou arquivo alterado
```

Isso permite reproduzir e corrigir o problema sem perder o estado da montagem.

---

# 23. Checklist final

Antes de considerar a validação concluída:

- [ ] firmware compila;
- [ ] upload via USB funciona;
- [ ] interface abre diretamente pelo ESP32;
- [ ] DHT22 testado individualmente;
- [ ] LDR testado individualmente;
- [ ] ventilador testado individualmente;
- [ ] exaustor testado individualmente;
- [ ] dois coolers funcionam simultaneamente;
- [ ] sensores continuam funcionando com os atuadores ativos;
- [ ] Dashboard validado;
- [ ] Histórico validado;
- [ ] Performance validada;
- [ ] cinco funções aparecem nos timings;
- [ ] tasks/stack aparecem;
- [ ] Wi-Fi/AP/RSSI aparecem;
- [ ] logs persistem e exportam;
- [ ] frequência dos sensores pode ser alterada;
- [ ] troca de Wi-Fi foi testada;
- [ ] interrupção de hardware foi testada;
- [ ] Light Sleep foi testado;
- [ ] OTA foi testado;
- [ ] persistência após reinício foi testada;
- [ ] histórico de pelo menos 24 horas foi comprovado;
- [ ] consumo de energia foi avaliado;
- [ ] evidências foram salvas;
- [ ] montagem final está organizada e segura;
- [ ] sistema foi colocado em caixa física antes da entrega.

---

## Resultado esperado da validação

Ao final, o EcoSense deve demonstrar que o ESP32 funciona como servidor ciberfísico autônomo, sem depender de Docker/PHP/MQTT para suas funções principais, integrando hardware real, firmware C++, FreeRTOS, interface Web embarcada, persistência, observabilidade, gerenciamento de energia e controle físico dos atuadores.
