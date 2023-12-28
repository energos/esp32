#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"

#define BUTTON_PIN CONFIG_KEY1_PIN
#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

#define BUTTON_DEBOUNCE_TICKS pdMS_TO_TICKS(10)

#define WIFI_SCAN_LIST_SIZE CONFIG_WIFI_SCAN_LIST_SIZE

#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t buttonTaskHandle = NULL;

void scan(void)
{
  uint16_t ap_num = 0;
  wifi_ap_record_t ap_records[WIFI_SCAN_LIST_SIZE];

  printf("Starting scan...\n");
  ESP_ERROR_CHECK(esp_wifi_scan_start(NULL, true));
  printf("Scan completed!\n");

  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

  /* print the list */
  printf("Found %d access points:\n", ap_num);
  printf(" SSID                          | Channel | RSSI | MAC\n");
  printf("-------------------------------|---------|------|------------------\n");
  for(int i = 0; i < ap_num; i++)
    printf(" %s\e[32G| %7d | %4d | %02x:%02x:%02x:%02x:%02x:%02x\n",\
           ap_records[i].ssid,\
           ap_records[i].primary,\
           ap_records[i].rssi,\
           *(ap_records[i].bssid),\
           *(ap_records[i].bssid + 1),\
           *(ap_records[i].bssid + 2),\
           *(ap_records[i].bssid + 3),\
           *(ap_records[i].bssid + 4),\
           *(ap_records[i].bssid + 5));
  printf("\n");
}

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
          led_status = !led_status;
          gpio_set_level(LED1_PIN, led_status);
          scan();
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

  xTaskCreate(button_task, "button_task", 4096, NULL, 10, &buttonTaskHandle);

  /* initialize NVS */
  /* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/nvs_flash.html */
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  /* initialize ESP-NETIF */
  /* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_netif.html */
  ESP_ERROR_CHECK(esp_netif_init());

  /* initialize Event Loop handling */
  /* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_event.html */
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  /* initialize WiFi */
  /* https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_wifi.html */
  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  printf("Hello, World!\n");

  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED2_PIN, led_state);
      led_state = !led_state;
      vTaskDelay(LED_DELAY_TICKS);
    }
}
