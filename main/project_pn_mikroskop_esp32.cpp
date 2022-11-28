#include <concepts>
#include <cstdio>
#include "esp_log.h"
#include "freertos/freeRTOS.h"
#include "freertos/task.h"
#include "wifi.h"

extern "C" void app_main() {
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  Wifi wifi{};
  wifi.connect();
  wifi.startReceiving();
}
