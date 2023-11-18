#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

void app_main(void)
{
  gpio_reset_pin(LED1_PIN);
  gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);

  gpio_reset_pin(LED2_PIN);
  gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

  printf("Hello, World!\n");
  while (1)
    {
      gpio_set_level(LED1_PIN, 1);
      printf("LED1 ON\n");
      gpio_set_level(LED2_PIN, 1);
      printf("LED2 ON\n");
      vTaskDelay(LED_DELAY_TICKS);

      gpio_set_level(LED1_PIN, 0);
      printf("LED1 OFF\n");
      gpio_set_level(LED2_PIN, 0);
      printf("LED2 OFF\n");
      vTaskDelay(LED_DELAY_TICKS);
    }
}
