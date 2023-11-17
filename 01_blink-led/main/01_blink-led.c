#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN CONFIG_LED_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

void app_main(void)
{
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  printf("Hello, World!\n");
  while (1)
    {
      gpio_set_level(LED_PIN, 1);
      printf("LED ON\n");
      vTaskDelay(LED_DELAY_TICKS);

      gpio_set_level(LED_PIN, 0);
      printf("LED OFF\n");
      vTaskDelay(LED_DELAY_TICKS);
    }
}
