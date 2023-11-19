#include <stdio.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)


TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

void task1(void *arg)
{
  int i = 0;
  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED1_PIN, led_state);
      printf("LED1 is %s : %8d\n", led_state == 1 ? "ON " : "OFF", i++);
      led_state = !led_state;
      vTaskDelay(LED_DELAY_TICKS);
    }
}

void task2(void *arg)
{
  double x = 0;
  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED2_PIN, led_state);
      printf("LED2 is %s : %8.0f %10.4f\n", led_state == 1 ? "ON " : "OFF", x, sqrt(x));
      x++;
      led_state = !led_state;
      vTaskDelay(LED_DELAY_TICKS);
    }
}

void app_main(void)
{
  gpio_reset_pin(LED1_PIN);
  gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);

  gpio_reset_pin(LED2_PIN);
  gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

  printf("Hello, World!\n");

  xTaskCreate(task1, "task1", 4096, NULL, 10, &myTask1Handle);
  xTaskCreate(task2, "task2", 4096, NULL, 10, &myTask2Handle);

  while (1)
    {
      printf("Stayin' Alive...\n");
      vTaskDelay(10 * LED_DELAY_TICKS);
    }
}
