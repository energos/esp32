#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_PIN CONFIG_KEY1_PIN
#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

#define BUTTON_DEBOUNCE_TICKS pdMS_TO_TICKS(10)

#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t ISR = NULL;

void IRAM_ATTR button_isr_handler(void *arg)
{
  xTaskResumeFromISR(ISR);
}

void button_task(void *arg)
{
  bool led_status = false;
  int count = 0;
  while (1)
    {
      vTaskSuspend(NULL);
      vTaskDelay(BUTTON_DEBOUNCE_TICKS);
      if (gpio_get_level(BUTTON_PIN) == 0)
        {
          led_status = !led_status;
          gpio_set_level(LED1_PIN, led_status);
          printf("Button pressed %d times!\n", ++count);
        }
    }
}

void app_main(void)
{
  // set pins as gpio
#if 0
  // undocumented? obsolete? replaced by gpio_reset_pin()?
  esp_rom_gpio_pad_select_gpio(BUTTON_PIN);
  esp_rom_gpio_pad_select_gpio(LED1_PIN);
  esp_rom_gpio_pad_select_gpio(LED2_PIN);
#else
  gpio_reset_pin(BUTTON_PIN);
  gpio_reset_pin(LED1_PIN);
  gpio_reset_pin(LED2_PIN);
#endif

  // set the correct direction
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

  // enable interrupt on falling  edge for button pin
  gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);

  printf("Hello, World!\n");

  xTaskCreate(button_task, "button_task", 4096, NULL, 10, &ISR);

  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED2_PIN, led_state);
      led_state = !led_state;
      // printf("Stayin' Alive...\n");
      vTaskDelay(LED_DELAY_TICKS);
    }
}
