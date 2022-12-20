#ifndef PROJECT_PN_MIKROSKOP_ESP32_PN_GLOBAL_H
#define PROJECT_PN_MIKROSKOP_ESP32_PN_GLOBAL_H

#include <esp_wifi_types.h>
#include "esp_event.h"
#include "nvs_flash.h"
#include "pn_config.h"

namespace pn {

inline void initStorage() noexcept {
  auto status = nvs_flash_init();

  if (
    status == ESP_ERR_NVS_NO_FREE_PAGES ||
    status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase()); // NOLINT
    status = nvs_flash_init();
  }

  ESP_ERROR_CHECK(status); // NOLINT
}

inline void initEventLoop() noexcept {
  ESP_ERROR_CHECK(esp_event_loop_create_default()); // NOLINT
}

inline void init() noexcept {
  initStorage();
  initEventLoop();
}

} // namespace pn

namespace pn::internal {
[[nodiscard]] consteval wifi_config_t getWifiConfig() noexcept {
  return wifi_config_t { .sta {
    .ssid     = PN_CONFIG_WIFI_SSID,
    .password = PN_CONFIG_WIFI_PASS,
    .scan_method = {},
    .bssid_set = {},
    .bssid = {},
    .channel = {},
    .listen_interval = {},
    .sort_method = {},
    .threshold = {
      .rssi = {},
      .authmode = PN_CONFIG_WIFI_AUTH,
    },
    .pmf_cfg = {
      .capable = true,
      .required = false,
    },
    .rm_enabled = {},
    .btm_enabled = {},
    .mbo_enabled = {},
    .ft_enabled = {},
    .owe_enabled = {},
    .reserved = {},
    .sae_pwe_h2e = {},
    .failure_retry_cnt = {},
  }};
}
} // namespace pn::internal

#endif // PROJECT_PN_MIKROSKOP_ESP32_PN_GLOBAL_H
