#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define LED_R_PIN 11 //led vermelho
#define LED_G_PIN 12 //led verde
#define LED_B_PIN 13 //led azul
#define JOYSTICK_X_PIN 27  // GPIO para eixo X
#define JOYSTICK_Y_PIN 26  // GPIO para eixo Y
#define JOYSTICK_PB 22 // GPIO para botão do Joystick
#define Botao_A 5 // GPIO para botão A

// Variáveis globais
volatile bool led_green_state = false;
volatile bool pwm_leds_enabled = true;
volatile uint16_t joystick_x_value = 2048;
volatile uint16_t joystick_y_value = 2048;
ssd1306_t ssd;


//Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

// Função de inicialização do PWM
void init_pwm() {

  const float DIVIDER_PWM = 16.0;
  const uint16_t PERIOD = 20000;

  // Configuração dos pinos de PWM para os LEDs RGB
  gpio_set_function(LED_R_PIN, GPIO_FUNC_PWM);
  gpio_set_function(LED_G_PIN, GPIO_FUNC_PWM);
  gpio_set_function(LED_B_PIN, GPIO_FUNC_PWM);

  // Obtenção dos slices de PWM
  uint slice_num_r = pwm_gpio_to_slice_num(LED_R_PIN);
  uint slice_num_g = pwm_gpio_to_slice_num(LED_G_PIN);
  uint slice_num_b = pwm_gpio_to_slice_num(LED_B_PIN);

  pwm_config config_r = pwm_get_default_config();
  pwm_config config_g = pwm_get_default_config();
  pwm_config config_b = pwm_get_default_config();

  pwm_config_set_clkdiv(&config_r, DIVIDER_PWM);
  pwm_config_set_clkdiv(&config_g, DIVIDER_PWM);
  pwm_config_set_clkdiv(&config_b, DIVIDER_PWM);

  // Configuração dos slices de PWM
  pwm_set_wrap(slice_num_r, 255);
  pwm_set_wrap(slice_num_g, 255);
  pwm_set_wrap(slice_num_b, 255);

  pwm_set_enabled(slice_num_r, true);
  pwm_set_enabled(slice_num_g, true);
  pwm_set_enabled(slice_num_b, true);
}

// Função de interrupção para o botão do joystick
void joystick_button_irq_handler(uint gpio, uint32_t events) {
  // Alternar estado do LED Verde
  led_green_state = !led_green_state;
  gpio_put(LED_G_PIN, led_green_state);

  // Modificar borda do display (implementar lógica de alternância de borda)

}

// Função de interrupção para o botão A
void button_a_irq_handler(uint gpio, uint32_t events) {
  // Alternar estado dos LEDs PWM
  pwm_leds_enabled = !pwm_leds_enabled;
  if (!pwm_leds_enabled) {
      pwm_set_gpio_level(LED_R_PIN, 0);
      pwm_set_gpio_level(LED_B_PIN, 0);
  }
}

int main()
{
  stdio_init_all();
  // Para ser utilizado o modo BOOTSEL com botão B
  gpio_init(botaoB);
  gpio_set_dir(botaoB, GPIO_IN);
  gpio_pull_up(botaoB);
  gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
  
  // Inicialização dos LEDs
  gpio_init(LED_R_PIN);
  gpio_init(LED_G_PIN); 
  gpio_init(LED_B_PIN);
  gpio_set_dir(LED_R_PIN, GPIO_OUT);
  gpio_set_dir(LED_G_PIN, GPIO_OUT);
  gpio_set_dir(LED_B_PIN, GPIO_OUT);
  gpio_put(LED_R_PIN, 1);
  gpio_put(LED_G_PIN, 1);
  gpio_put(LED_B_PIN, 1);
  
  // Inicialização do PWM
  init_pwm();
  
  

  gpio_init(JOYSTICK_PB);
  gpio_set_dir(JOYSTICK_PB, GPIO_IN);
  gpio_pull_up(JOYSTICK_PB);
  gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &joystick_button_irq_handler); 

  gpio_init(Botao_A);
  gpio_set_dir(Botao_A, GPIO_IN);
  gpio_pull_up(Botao_A);
  gpio_set_irq_enabled_with_callback(JOYSTICK_PB, GPIO_IRQ_EDGE_FALL, true, &button_a_irq_handler);

  // I2C Initialisation. Using it at 400Khz.
  i2c_init(I2C_PORT, 400 * 1000);

  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C); // Set the GPIO pin function to I2C
  gpio_pull_up(I2C_SDA); // Pull up the data line
  gpio_pull_up(I2C_SCL); // Pull up the clock line
  ssd1306_t ssd; // Inicializa a estrutura do display
  ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
  ssd1306_config(&ssd); // Configura o display
  ssd1306_send_data(&ssd); // Envia os dados para o display

  // Limpa o display. O display inicia com todos os pixels apagados.
  ssd1306_fill(&ssd, false);
  ssd1306_send_data(&ssd);

  adc_init();
  adc_gpio_init(JOYSTICK_X_PIN);
  adc_gpio_init(JOYSTICK_Y_PIN);  
  


  int adc_value_x;
  int adc_value_y;
  char str_x[5];  // Buffer para armazenar a string
  char str_y[5];  // Buffer para armazenar a string  
  
  bool cor = true;
  while (true)
  {
    adc_select_input(0); // Seleciona o ADC para eixo X. O pino 26 como entrada analógica
    adc_value_x = adc_read();
    adc_select_input(1); // Seleciona o ADC para eixo Y. O pino 27 como entrada analógica
    adc_value_y = adc_read();    
    sprintf(str_x, "%d", adc_value_x);  // Converte o inteiro em string
    sprintf(str_y, "%d", adc_value_y);  // Converte o inteiro em string
    
    // Atualização do display SSD1306 (implementar lógica de movimento do quadrado)
    int pos_x = (adc_value_x * (WIDTH - 8)) / 2048;
    int pos_y = (adc_value_y * (HEIGHT - 8)) / 2048;
    
    // Ajuste do brilho do LED Azul conforme o valor do eixo Y
    uint16_t pwm_value_blue = (adc_value_y * 255) / 4095;
    pwm_set_gpio_level(LED_G_PIN, pwm_value_blue);

    // Ajuste do brilho do LED Vermelha conforme o valor do eixo Y
    uint16_t pwm_value_red = (adc_value_x * 255) / 4095;
    pwm_set_gpio_level(LED_B_PIN, pwm_value_red);

    
    cor = !cor;
    // Atualiza o conteúdo do display com animações
    ssd1306_fill(&ssd, !cor); // Limpa o display
    ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor); // Desenha um retângulo
    ssd1306_rect(&ssd, pos_y, pos_x, 8, 8, true, false); // Desenha um retângulo       
    ssd1306_send_data(&ssd); // Atualiza o display


    sleep_ms(100);
  }
}