#include <cstdio>
#include "freertos/freeRTOS.h"
#include "Wifi.h"
#include "Motors.h"

extern "C" void app_main() {
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  Motors::startTasks();
  Wifi::startTask();
}
