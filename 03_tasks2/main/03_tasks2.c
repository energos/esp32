#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)


TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

const char *eTaskStateToString(eTaskState state)
{
  switch (state)
    {
    case eRunning:
      return "Running";
    case eReady:
      return "Ready";
    case eBlocked:
      return "Blocked";
    case eSuspended:
      return "Suspended";
    case eDeleted:
      return "Deleted";
    default:
      return "Invalid";
    }
}

void task1(void *arg)
{
  uint8_t led_state = 0;
  int c = 0;
  eTaskState task2State = eInvalid;
  char *task2Name = "NoName";
  while (1)
    {
      c = xTaskGetTickCount();
      gpio_set_level(LED1_PIN, led_state);
      led_state = !led_state;
      if (c == 1003)
        {
          vTaskSuspend(myTask2Handle);
          printf("=== Task2 Suspended! ===\n");
        }
      if (c == 2003)
        {
          vTaskResume(myTask2Handle);
          printf("=== Task2 Resumed!   ===\n");
        }
      if (c == 3003)
        {
          vTaskDelete(myTask2Handle);
          printf("=== Task2 Deleted!   ===\n");
        }
      if (c > 100)
        {
          task2State = eTaskGetState(myTask2Handle);
          task2Name = pcTaskGetName(myTask2Handle);
        }
      printf("Hello from %s: %s is %s [%d]\n", pcTaskGetName(NULL), task2Name, eTaskStateToString(task2State), c);
      vTaskDelay(LED_DELAY_TICKS);
    }
}

void task2(void *arg)
{
  uint8_t led_state = 0;
  int c = 0;
  while (1)
    {
      c = xTaskGetTickCount();
      gpio_set_level(LED2_PIN, led_state);
      led_state = !led_state;
      printf("Hello from %s [%d]\n", pcTaskGetName(NULL), c);
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
