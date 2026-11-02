# Checklist de demonstração — ESP32 Server

## Antes da apresentação

- [ ] atualizar `secrets.h` local;
- [ ] compilar `pio run`;
- [ ] fazer upload no ESP32;
- [ ] confirmar sensores físicos;
- [ ] confirmar dois atuadores físicos;
- [ ] confirmar caixa/montagem;
- [ ] preencher nomes e e-mails na página Sobre;
- [ ] deixar `firmware.bin` disponível para demonstrar OTA;
- [ ] limpar logs antigos se necessário;
- [ ] deixar o dispositivo ligado por tempo suficiente para gerar histórico.

## Roteiro de demonstração

1. Mostrar que o ESP32 inicia sozinho.
2. Abrir a interface pelo IP do próprio ESP32.
3. Mostrar valores atuais dos sensores.
4. Abrir Performance e mostrar:
   - CPU;
   - heap;
   - stack das tasks;
   - flash/PSRAM;
   - quantidade de tasks;
   - tempos de funções;
   - Wi-Fi/RSSI/AP;
   - sensores e gráfico de 24h;
   - OTA.
5. Alterar a frequência de leitura pela interface.
6. Mostrar logs e exportar o arquivo.
7. Acionar fan e exhaust.
8. Pressionar o botão de interrupção e mostrar o evento no log.
9. Demonstrar Light Sleep.
10. Se possível, alterar a rede Wi-Fi pela interface.
11. Desligar PHP/PostgreSQL/MQTT externo e comprovar que o servidor local continua operacional.

## Evidências para o relatório

Capture screenshots/fotos de:

- montagem física;
- dashboard embarcado;
- página Performance;
- gráfico de histórico;
- tasks/stacks;
- logs;
- configuração;
- OTA;
- acionamentos;
- testes automatizados;
- GitHub Actions verde;
- monitor serial;
- consumo de energia antes/depois do Light Sleep.
