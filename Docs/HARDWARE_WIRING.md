# Montagem física recomendada — EcoSense

Este guia corresponde aos pinos configurados em `esp32/include/AppConfig.h`.

## Componentes mínimos

- 1x ESP32 DevKit;
- 1x DHT22;
- 1x LDR + resistor para divisor de tensão;
- 2x módulos relé compatíveis com lógica 3,3 V;
- 1x ventilador DC de baixa tensão;
- 1x segundo dispositivo DC de acionamento (por exemplo, outro ventilador/exaustor);
- fonte adequada aos atuadores;
- jumpers e protoboard/placa de montagem;
- caixa física ventilada e segura.

O firmware também possui entrada analógica opcional para sensor de qualidade do ar.

## Entradas

### DHT22

| DHT22 | ESP32 |
|---|---|
| VCC | 3V3 |
| DATA | GPIO 4 |
| GND | GND |

Use resistor de pull-up adequado no pino DATA quando o módulo utilizado não possuir um integrado.

### LDR

O LDR deve ser usado em divisor de tensão, com o ponto central ligado ao GPIO 34 (ADC).

Não aplique tensão acima do limite permitido pelo ESP32 no pino analógico.

### Qualidade do ar opcional

A entrada configurada é GPIO 35.

Verifique a tensão de saída do módulo antes de ligar ao ESP32. Muitos módulos trabalham em 5 V e podem exigir divisor de tensão/condicionamento.

## Saídas

| Função | GPIO |
|---|---:|
| Relé do ventilador | 26 |
| Relé do exaustor | 27 |

O ESP32 controla apenas a entrada lógica do módulo relé. A alimentação dos motores/ventiladores deve ser feita por fonte apropriada.

Para a apresentação acadêmica, prefira atuadores DC de baixa tensão. Não monte cargas de rede 127/220 V sem projeto elétrico, isolamento e supervisão adequados.

## Interrupção de hardware

GPIO 0 (botão BOOT) é usado como interrupção de demonstração.

Durante a execução normal, um toque curto no botão deve incrementar o contador de interrupções e gerar uma entrada no log. Não mantenha o botão pressionado durante reset/boot.

## Checklist antes de energizar

- conferir GND comum quando necessário;
- conferir tensão de alimentação dos sensores;
- conferir se o módulo relé aceita lógica 3,3 V;
- conferir polaridade e fonte dos atuadores;
- manter cabos de potência separados dos sinais;
- garantir que a caixa permita ventilação;
- não deixar terminais expostos;
- validar um componente por vez.

## Validação sugerida

1. ESP32 inicia e serve a página Web.
2. DHT22 apresenta temperatura/umidade válidas.
3. LDR reage ao cobrir/expor à luz.
4. Fan ON/OFF aciona o primeiro relé.
5. Exhaust ON/OFF aciona o segundo relé.
6. Botão BOOT gera log de interrupção.
7. Desligar backend externo não derruba o servidor local.
