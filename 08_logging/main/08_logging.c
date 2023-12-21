#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define BUTTON_PIN CONFIG_KEY1_PIN
#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

#define BUTTON_DEBOUNCE_TICKS pdMS_TO_TICKS(10)

#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t buttonTaskHandle = NULL;
TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

static const char *TAG = "08_logging";

void button_isr_handler(void *arg)
{
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
#if 1
          ESP_LOG_LEVEL(esp_log_level_get(TAG), TAG, "Buttom pressed!");
#else
          ESP_LOGV(TAG, "Button pressed");
          ESP_LOGD(TAG, "Button very pressed!");
          ESP_LOGE(TAG, "Button f***ing seriously pressed!!!");
#endif
          led_status = !led_status;
          gpio_set_level(LED1_PIN, led_status);
        }
    }
}

void task1(void *arg)
{
  char buffer[20] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x11, 0x22, 0x33, 0x44, 0x55};

  while (1)
    {
      ESP_LOGI(TAG, "Hello from task1");
      ESP_LOG_BUFFER_HEX(TAG, buffer, sizeof buffer);
      /* ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, sizeof buffer, ESP_LOG_WARN); */
      printf( "timestamp = %u, tick count = %u\n", (unsigned int) esp_log_timestamp(), (unsigned int) xTaskGetTickCount());
      vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void task2(void *arg)
{
  while (1)
    {
      ESP_LOGW(TAG, "Hello from task2");
      vTaskDelay(pdMS_TO_TICKS(5000));
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

  esp_log_level_set(TAG, ESP_LOG_DEBUG);

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
