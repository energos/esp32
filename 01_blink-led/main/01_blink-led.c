#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_PIN GPIO_NUM_2
#define LED_DELAY pdMS_TO_TICKS(500)

void app_main(void)
{
  gpio_reset_pin(LED_PIN);
  gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

  printf("Hello, World!\n");
  while (1)
    {
      gpio_set_level(LED_PIN, 1);
      printf("LED ON\n");
      vTaskDelay(LED_DELAY);

      gpio_set_level(LED_PIN, 0);
      printf("LED OFF\n");
      vTaskDelay(LED_DELAY);
    }
}
