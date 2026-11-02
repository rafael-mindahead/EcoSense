# Montagem física recomendada — EcoSense

Este guia corresponde aos pinos configurados em `esp32/include/AppConfig.h` e ao hardware escolhido para a montagem física do EcoSense.

## Componentes da montagem

- 1x ESP32 DevKit WROOM-32;
- 1x DHT22 / AM2302;
- 1x módulo LDR com saída analógica;
- 1x sensor de qualidade do ar ENS160 + AHT21 planejado para a etapa de integração;
- 2x módulos MOSFET D4184 compatíveis com sinal lógico de 3,3 V;
- 2x coolers/ventoinhas DC de 12 V;
- 1x fonte externa 12 V / 2 A;
- 1x conector P4 fêmea com borne;
- 2x protoboards de 830 pontos;
- jumpers macho-macho e macho-fêmea;
- caixa física ventilada e segura para a entrega final.

## Entradas

### DHT22

| DHT22 | ESP32 |
|---|---|
| VCC | 3V3 |
| DATA | GPIO 4 |
| GND | GND |

Use resistor de pull-up adequado no pino DATA quando o módulo utilizado não possuir um integrado.

### LDR

O módulo LDR deve utilizar a saída analógica `AO`, ligada ao GPIO 34 (ADC).

| LDR | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| AO | GPIO 34 |

O módulo escolhido aceita alimentação de 3,3 V a 5 V. Para ligação direta ao ADC do ESP32, a montagem deve operar o módulo em 3,3 V para que a saída analógica permaneça compatível com o microcontrolador.

### Qualidade do ar

A versão inicial do firmware ainda possui uma entrada analógica em GPIO 35 para qualidade do ar. O hardware planejado para a montagem final é um módulo ENS160 + AHT21, que usa barramento I2C. A integração desse módulo será feita em uma etapa própria antes da montagem final; até lá, GPIO 35 permanece apenas como compatibilidade com a implementação anterior.

## Saídas e acionamento

Os relés originalmente previstos foram substituídos por dois drivers MOSFET D4184. O módulo escolhido aceita sinal lógico a partir de 3,3 V, portanto o ESP32 pode comandá-lo diretamente no lado lógico.

| Função | GPIO | Driver | Carga |
|---|---:|---|---|
| Ventilador (fan) | 26 | D4184 #1 | Cooler 12 V #1 |
| Exaustor (exhaust) | 27 | D4184 #2 | Cooler 12 V #2 |

O firmware utiliza os nomes `FAN_DRIVER_PIN` e `EXHAUST_DRIVER_PIN` para refletir o hardware real.

### Alimentação dos atuadores

Os coolers não devem ser alimentados pelo ESP32 nem pela protoboard.

Fluxo de potência:

```text
Fonte 12 V / 2 A
   ├── D4184 #1 ──→ Cooler 12 V #1
   └── D4184 #2 ──→ Cooler 12 V #2
```

Fluxo de controle:

```text
ESP32 GPIO 26 ──→ entrada lógica D4184 #1
ESP32 GPIO 27 ──→ entrada lógica D4184 #2
```

Use GND comum entre o ESP32 e o lado de controle dos módulos D4184 quando exigido pela montagem do módulo. Os 12 V da fonte externa nunca devem ser aplicados a um GPIO do ESP32.

O conector P4 fêmea com borne pode ser usado para transformar o plug da fonte em terminais de alimentação, facilitando a distribuição dos 12 V aos dois drivers.

## Interrupção de hardware

GPIO 0 (botão BOOT) é usado como interrupção de demonstração.

Durante a execução normal, um toque curto no botão deve incrementar o contador de interrupções e gerar uma entrada no log. Não mantenha o botão pressionado durante reset/boot.

## Checklist antes de energizar

- conferir a polaridade da fonte de 12 V;
- confirmar a corrente nominal dos dois coolers e manter a soma com margem abaixo da capacidade da fonte;
- nunca conectar 12 V ou 5 V diretamente aos GPIOs do ESP32;
- conferir GND comum do circuito de controle quando necessário;
- conferir a tensão de alimentação dos sensores;
- confirmar que cada D4184 responde corretamente ao sinal lógico de 3,3 V;
- não conduzir a corrente dos coolers pela protoboard;
- manter cabos de potência separados dos sinais;
- garantir ventilação adequada na caixa;
- não deixar terminais de potência expostos;
- validar um componente por vez antes de integrar o conjunto.

## Validação sugerida

1. ESP32 inicia e serve a página Web.
2. DHT22 apresenta temperatura/umidade válidas.
3. LDR reage ao cobrir/expor à luz.
4. Fan ON/OFF comuta o D4184 #1 e o cooler #1.
5. Exhaust ON/OFF comuta o D4184 #2 e o cooler #2.
6. Validar o sensor de qualidade do ar após a integração I2C do ENS160 + AHT21.
7. Botão BOOT gera log de interrupção.
8. Desligar backend externo não derruba o servidor local.
