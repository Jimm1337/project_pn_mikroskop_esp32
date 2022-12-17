#include "freertos/freeRTOS.h"
#include "pn_global.h"
#include "Wifi.h"
#include "Motors.h"

extern "C" void app_main() {
  pn::init();
  Motors::startTask();
  Wifi::startTask();
}
