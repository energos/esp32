#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"

#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
QueueHandle_t queue1;

void task1(void *arg)
{
  char txbuff[50];
  queue1 = xQueueCreate(5, sizeof txbuff);
  if (queue1 == 0)
    {
      printf("Failed to create queue1\n");
    }

  sprintf(txbuff,"Hello, World! 1");
  xQueueSend(queue1, txbuff, 0);

  sprintf(txbuff,"Hello, World! 2");
  xQueueSend(queue1, txbuff, 0);

  sprintf(txbuff,"Hello, World! 3");
  xQueueSend(queue1, txbuff, 0);

  sprintf(txbuff,"Hello, World! 4");
  xQueueSendToFront(queue1, txbuff, 0);

  while (1)
    {
      printf("Data waiting to be read: %d  - Available spaces: %d\n", uxQueueMessagesWaiting(queue1), uxQueueSpacesAvailable(queue1));
      vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void task2(void *arg)
{
  char rxbuff[50];

  while (1)
    {
      if (xQueueReceive(queue1, rxbuff, 5))
        {
          printf("    Got data from queue!    >>>%s<<<\n", rxbuff);
        }
      vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void app_main(void)
{
  gpio_reset_pin(LED2_PIN);
  gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

  printf( "Hello, World!\n");

  xTaskCreate(task1, "task1", 4096, NULL, 10, &myTask1Handle);
  xTaskCreate(task2, "task2", 4096, NULL, 10, &myTask2Handle);

  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED2_PIN, led_state);
      led_state = !led_state;
      vTaskDelay(LED_DELAY_TICKS);
    }
}
