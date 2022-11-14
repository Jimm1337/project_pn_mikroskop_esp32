#include "wifi.h"
#include <cstring>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"

Wifi::Wifi() noexcept:
  m_wifiConfig{ [] {
    wifi_config_t config{};
    std::strncpy(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<char*>(config.sta.ssid),
      SSID.data(),
      SSID.size() + 1);
    std::strncpy(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<char*>(config.sta.password),
      PASSWORD.data(),
      PASSWORD.size() + 1);
    config.sta.threshold.authmode = AUTH_MODE;
    config.sta.pmf_cfg.capable    = true;
    config.sta.pmf_cfg.required   = false;
    return config;
  }() } {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();
}

void Wifi::connect() noexcept {
  m_wifiEventGroup = xEventGroupCreate();

  connectRegisterHandlers();
  connectSetup();
  ESP_ERROR_CHECK(connectWaitForConnection());

  vEventGroupDelete(m_wifiEventGroup);
}

void Wifi::connectRegisterHandlers() noexcept {
  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi::wifiEventHandler, nullptr, nullptr));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi::ipEventHandler, nullptr, nullptr));
}

void Wifi::connectSetup() noexcept {
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &m_wifiConfig));

  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(LOG_TAG.data(), "Wifi setup complete");
}

esp_err_t Wifi::connectWaitForConnection() noexcept {
  static constexpr EventBits_t waitCondition = Status::SUCCESS | Status::FAIL;

  EventBits_t bits = xEventGroupWaitBits(
    m_wifiEventGroup, waitCondition, pdFALSE, pdFALSE, portMAX_DELAY);

  if ((bits & Status::SUCCESS) != 0) {
    ESP_LOGI(LOG_TAG.data(), "Connected to ap");
    return Status::SUCCESS;
  } else if ((bits & Status::FAIL) != 0) {
    ESP_LOGI(LOG_TAG.data(), "Failed to connect to ap");
    return Status::FAIL;
  }

  ESP_LOGE(LOG_TAG.data(), "UNEXPECTED EVENT");
  return Status::FAIL;
}

void Wifi::wifiEventHandler(
  void*            arg,
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            eventData) noexcept {
  // todo
}

void Wifi::ipEventHandler(
  void*            arg,
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            eventData) noexcept {
  // todo
}
