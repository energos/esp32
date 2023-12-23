#if 1

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define BUTTON_PIN CONFIG_KEY1_PIN
#define LED1_PIN CONFIG_LED1_PIN
#define LED2_PIN CONFIG_LED2_PIN
#define LED_DELAY_TICKS pdMS_TO_TICKS(CONFIG_LED_DELAY)

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

  printf("Hello, World!\n");

  uint8_t led_state = 0;
  while (1)
    {
      gpio_set_level(LED2_PIN, led_state);
      led_state = !led_state;
      vTaskDelay(LED_DELAY_TICKS);
    }
}

#else

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

void scann(){
  // configure and run the scan process in blocking mode
  wifi_scan_config_t scan_config = {
    .ssid = 0,
    .bssid = 0,
    .channel = 0,
    .show_hidden = true
  };
  printf("Start scanning...");
  ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
  printf(" completed!\n");

  // get the list of APs found in the last scan
  uint16_t ap_num;
  wifi_ap_record_t ap_records[20];
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
  ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num, ap_records));

  // print the list
  printf("Found %d access points:\n", ap_num);

  printf("               SSID              | Channel | RSSI |   MAC \n\n");
  //printf("----------------------------------------------------------------\n");
  for(int i = 0; i < ap_num; i++)
    printf("%32s | %7d | %4d   %2x:%2x:%2x:%2x:%2x:%2x   \n", ap_records[i].ssid, ap_records[i].primary, ap_records[i].rssi , *ap_records[i].bssid, *(ap_records[i].bssid+1), *(ap_records[i].bssid+2), *(ap_records[i].bssid+3), *(ap_records[i].bssid+4), *(ap_records[i].bssid+5));
  //  printf("----------------------------------------------------------------\n");

}


void app_main()
{
  // initialize NVS
  ESP_ERROR_CHECK(nvs_flash_init());

  esp_netif_init();

  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());// starts wifi usage



  while(1) {  vTaskDelay(3000 / portTICK_PERIOD_MS);
    scann();
  }
}

#endif
