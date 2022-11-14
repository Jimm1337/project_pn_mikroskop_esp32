#include "wifi.h"
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

Wifi::Wifi() noexcept {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_sta();
}

void Wifi::connect() noexcept {
  m_wifiEventGroup = xEventGroupCreate();

  connectRegisterHandlers();
  connectSetup();
  ESP_ERROR_CHECK(static_cast<int>(connectWaitForConnection()));

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

Wifi::Status Wifi::connectWaitForConnection() noexcept {
  static constexpr EventBits_t waitCondition =
    static_cast<EventBits_t>(Status::SUCCESS) |
    static_cast<EventBits_t>(Status::FAIL);

  EventBits_t bits = xEventGroupWaitBits(
    m_wifiEventGroup, waitCondition, pdFALSE, pdFALSE, portMAX_DELAY);

  const bool success = (bits & static_cast<EventBits_t>(Status::SUCCESS)) != 0;
  const bool fail    = (bits & static_cast<EventBits_t>(Status::FAIL)) != 0;

  if (success) {
    ESP_LOGI(LOG_TAG.data(), "Connected to ap");
    return Status::SUCCESS;
  } else if (fail) {
    ESP_LOGI(LOG_TAG.data(), "Failed to connect to ap");
    return Status::FAIL;
  }

  ESP_LOGE(LOG_TAG.data(), "UNEXPECTED EVENT");
  return Status::FAIL;
}

void Wifi::wifiEventHandler(
  void*            arg,
  esp_event_base_t eventBase,
  int32_t          eventId,
  void*            eventData) noexcept {
  // todo
}

void Wifi::ipEventHandler(
  void*            arg,
  esp_event_base_t eventBase,
  int32_t          eventId,
  void*            eventData) noexcept {
  // todo
}
