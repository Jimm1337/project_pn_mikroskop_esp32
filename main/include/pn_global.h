#ifndef PROJECT_PN_MIKROSKOP_ESP32_PN_GLOBAL_H
#define PROJECT_PN_MIKROSKOP_ESP32_PN_GLOBAL_H

#include "esp_event.h"
#include "nvs_flash.h"

namespace pn {

inline void initStorage() noexcept {
  auto status = nvs_flash_init();

  if (
    status == ESP_ERR_NVS_NO_FREE_PAGES ||
    status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase()); //NOLINT
    status = nvs_flash_init();
  }

  ESP_ERROR_CHECK(status); //NOLINT
}

inline void initEventLoop() noexcept {
  ESP_ERROR_CHECK(esp_event_loop_create_default()); //NOLINT
}

inline void init() noexcept {
  initStorage();
  initEventLoop();
}

} // namespace pn

#endif // PROJECT_PN_MIKROSKOP_ESP32_PN_GLOBAL_H
