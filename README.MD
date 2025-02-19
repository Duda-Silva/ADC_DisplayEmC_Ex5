# Projeto: Interface Homem-Máquina (IHM) com Display OLED e Joystick

## Descrição

Este projeto utiliza o display OLED SSD1306 com resolução de 128x64 pixels e o microcontrolador RP2040 (Raspberry Pi Pico) para exibir informações do conversor analógico-digital (ADC) e do Joystick. O projeto também mostra a leitura dos botões do Joystick e do botão A, além de controlar o brilho dos LEDs RGB com base na posição do joystick.

## Funcionalidades

- Exibição de informações do ADC no display OLED.
- Movimento de um quadrado no display conforme a posição do joystick.
- Controle do brilho do LED Azul conforme o valor do eixo Y do joystick.
- Controle do brilho do LED Vermelho conforme o valor do eixo X do joystick.
- Alternância do estado do LED Verde a cada acionamento do botão do joystick.
- Alternância do estado dos LEDs PWM a cada acionamento do botão A.

## Hardware Utilizado

- Raspberry Pi Pico (RP2040)
- Display OLED SSD1306 (128x64 pixels)
- Joystick com botão
- LEDs RGB (Vermelho, Verde e Azul)
- Resistores e jumpers para conexões

## Conexões

- I2C SDA: GPIO 14
- I2C SCL: GPIO 15
- LED Vermelho: GPIO 11
- LED Verde: GPIO 12
- LED Azul: GPIO 13
- Joystick eixo X: GPIO 27
- Joystick eixo Y: GPIO 26
- Botão do Joystick: GPIO 22
- Botão A: GPIO 5
- Botão B (modo BOOTSEL): GPIO 6

## Código

O código principal está no arquivo `ADC_DisplayC.c`. Ele inclui a configuração do ADC, PWM, I2C e interrupções para os botões. O loop principal lê os valores do joystick e atualiza o display e os LEDs conforme necessário.

## Compilação e Execução

1. Clone o repositório para o seu ambiente de desenvolvimento.
2. Certifique-se de ter o SDK do Raspberry Pi Pico configurado.
3. Compile o código utilizando o CMake.
4. Carregue o binário gerado no Raspberry Pi Pico.

## Licença

Este projeto está licenciado sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.

## video

link: [Link do video](https://youtube.com/shorts/qqFqmbn6B7A?feature=share)

