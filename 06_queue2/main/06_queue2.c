#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "freertos/queue.h"

#define BUTTON_PIN CONFIG_KEY1_PIN
#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

#define BUTTON_DEBOUNCE_TICKS pdMS_TO_TICKS(10)

#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t buttonTaskHandle = NULL;
TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;
QueueHandle_t queue1;

void button_isr_handler(void *arg)
{
  char txbuff[50];
  sprintf(txbuff,"Hello from button ISR!");
  /* No debouncing. Not good! */
  xQueueSendFromISR(queue1, txbuff, NULL);

  xTaskResumeFromISR(buttonTaskHandle);
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

void task1(void *arg)
{
  char txbuff[50];

  queue1 = xQueueCreate(1, sizeof txbuff);

  if (queue1 == 0)
    {
      printf("Failed to create queue1\n");
    }

  sprintf(txbuff,"Hello, World! 1");
  if (xQueueSend(queue1, txbuff, 0) != 1)
    {
      printf("Could not send message! >>>%s<<<\n", txbuff);
    }

  sprintf(txbuff,"Hello, World! 2");
  if (xQueueSend(queue1, txbuff, 0) != 1)
    {
      printf("Could not send message! >>>%s<<<\n", txbuff);
    }

  sprintf(txbuff,"Hello, World! 3");
  if (xQueueOverwrite(queue1, txbuff) != 1)
    {
      printf("Could not send message! >>>%s<<<\n", txbuff);
    }

  while (1)
    {
      printf("Data waiting to be read: %d - Available spaces: %d\n", uxQueueMessagesWaiting(queue1), uxQueueSpacesAvailable(queue1));
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task2(void *arg)
{
  char rxbuff[50];
  while (1)
    {
      if (xQueueReceive(queue1, rxbuff , 5))
        {
          printf("Got data from queue!    >>>%s<<<\n", rxbuff);
        }
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
  // set pins as gpio
  gpio_reset_pin(BUTTON_PIN);
  gpio_reset_pin(LED1_PIN);
  gpio_reset_pin(LED2_PIN);

  // set the correct direction
  gpio_set_direction(BUTTON_PIN, GPIO_MODE_INPUT);
  gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

  // enable interrupt on falling  edge for button pin
  gpio_set_intr_type(BUTTON_PIN, GPIO_INTR_NEGEDGE);
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);

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
