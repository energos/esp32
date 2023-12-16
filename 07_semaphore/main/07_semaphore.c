#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/semphr.h"

#define BUTTON_PIN CONFIG_KEY1_PIN
#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

#define BUTTON_DEBOUNCE_TICKS pdMS_TO_TICKS(10)

#define ESP_INTR_FLAG_DEFAULT 0

#define WAIT_FOR_BUTTON

TaskHandle_t buttonTaskHandle = NULL;
TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
SemaphoreHandle_t xSemaphore = NULL;

void button_isr_handler(void *arg)
{
  /* No debouncing. Not good! */
  /* xSemaphoreGiveFromISR(xSemaphore, NULL); */

  xTaskResumeFromISR(buttonTaskHandle);
}

void button_task(void *arg)
{
  bool led_status = false;
  while (1)
    {
      vTaskSuspend(NULL);
      vTaskDelay(BUTTON_DEBOUNCE_TICKS);
      if (gpio_get_level(BUTTON_PIN) == 0)
        {
          led_status = !led_status;
          gpio_set_level(LED1_PIN, led_status);
          xSemaphoreGive(xSemaphore);
        }
    }
}

void task1(void *arg)
{
#ifndef WAIT_FOR_BUTTON
  int n = 0;
#endif

  while (1)
    {
#ifndef WAIT_FOR_BUTTON
      printf("Sent %dth message at %u from task1\n", ++n, (unsigned int) xTaskGetTickCount());
      xSemaphoreGive(xSemaphore);
#endif
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task2(void *arg)
{
  int n = 0;
  while (1)
    {
#ifdef WAIT_FOR_BUTTON
      printf("Waiting for the button...\n");
#endif
      if (xSemaphoreTake(xSemaphore, portMAX_DELAY))
        {
          printf("Got  %dth message at %u\n", ++n, (unsigned int) xTaskGetTickCount());
        }
      else
        {
          printf("Timeout at %u\n", (unsigned int) xTaskGetTickCount());
          vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

void app_main(void)
{
  /* set pins as gpio */
  gpio_reset_pin(BUTTON_PIN);
  gpio_reset_pin(LED1_PIN);
  gpio_reset_pin(LED2_PIN);

  /* set pins direction */
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

  /* enable interrupt on falling  edge for button pin */
  gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);

  xSemaphore = xSemaphoreCreateBinary();

  printf("Hello, World!\n");

  xTaskCreate(task1, "task1", 4096, NULL, 10, &myTask1Handle);
  xTaskCreate(task2, "task2", 4096, NULL, 10, &myTask2Handle);
  xTaskCreate(button_task, "button_task", 4096, NULL, 10, &buttonTaskHandle);

  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED2_PIN, led_state);
      led_state = !led_state;
      vTaskDelay(LED_DELAY_TICKS);
    }
}
