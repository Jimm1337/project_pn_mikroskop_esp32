#include "wifi.h"
#include <array>
#include <cstring>
#include <source_location>
#include <string>
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

#define WIFI_ERR_CHECK(err)                                                    \
  if (err != ESP_OK) {                                                         \
    ESP_LOGE(                                                                  \
      Wifi::LOG_TAG.data(),                                                    \
      "Error in: %s",                                                          \
      std::source_location::current().function_name());                        \
    return Status::FAIL;                                                       \
  }

namespace Status {
inline constexpr esp_err_t SUCCESS = BIT0;
inline constexpr esp_err_t FAIL    = BIT1;
} // namespace Status

Wifi::Wifi() noexcept {
  std::strncpy(
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    reinterpret_cast<char*>(m_wifiConfig.sta.ssid),
    SSID.data(),
    SSID.size() + 1);
  std::strncpy(
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    reinterpret_cast<char*>(m_wifiConfig.sta.password),
    PASSWORD.data(),
    PASSWORD.size() + 1);
  m_wifiConfig.sta.threshold.authmode = AUTH_MODE;
  m_wifiConfig.sta.pmf_cfg.capable    = true;
  m_wifiConfig.sta.pmf_cfg.required   = false;

  ESP_ERROR_CHECK(esp_netif_init());
  esp_netif_create_default_wifi_sta();
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Wifi::connect() noexcept {
  s_wifiEventGroup = xEventGroupCreate();
  if (s_wifiEventGroup == nullptr) {
    ESP_LOGE(LOG_TAG.data(), "Failed to create event group"); // NOLINT
    return;
  }

  ESP_ERROR_CHECK(connectRegisterHandlers());
  ESP_ERROR_CHECK(connectSetup());
  ESP_ERROR_CHECK(connectWaitForConnection());

  vEventGroupDelete(s_wifiEventGroup);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void Wifi::startReceiving() noexcept {
  const sockaddr_in destAddr = [] {
    sockaddr_in addr{};
    addr.sin_addr.s_addr = inet_addr(SERVER_IP.data());
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(SERVER_PORT);
    return addr;
  }();
  // inet_ntoa(destAddr.sin_addr);

  TcpSocket socket{};
  ESP_ERROR_CHECK(tcpCreateSocket(socket));
  ESP_ERROR_CHECK(tcpConnectServer(socket, &destAddr));

  tcpReceiveData(socket); // inf loop
}

esp_err_t Wifi::connectRegisterHandlers() noexcept {
  esp_err_t err = Status::SUCCESS;

  err = esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi::eventHandlerWifi, nullptr, nullptr);
  WIFI_ERR_CHECK(err); // NOLINT

  err = esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi::eventHandlerIp, nullptr, nullptr);
  WIFI_ERR_CHECK(err); // NOLINT

  return err;
}

esp_err_t Wifi::connectSetup() noexcept {
  esp_err_t err = Status::SUCCESS;

  err = esp_wifi_set_mode(WIFI_MODE_STA);
  WIFI_ERR_CHECK(err); // NOLINT

  err = esp_wifi_set_config(WIFI_IF_STA, &m_wifiConfig);
  WIFI_ERR_CHECK(err); // NOLINT

  err = esp_wifi_start();
  WIFI_ERR_CHECK(err); // NOLINT

  ESP_LOGI(LOG_TAG.data(), "Wifi setup complete"); // NOLINT
  return err;
}

esp_err_t Wifi::connectWaitForConnection() noexcept {
  // NOLINTNEXTLINE
  static constexpr EventBits_t waitCondition = Status::SUCCESS | Status::FAIL;

  const EventBits_t bits = xEventGroupWaitBits(
    s_wifiEventGroup, waitCondition, pdFALSE, pdFALSE, portMAX_DELAY);

  if ((bits & Status::SUCCESS) != 0) {           // NOLINT
    ESP_LOGI(LOG_TAG.data(), "Connected to ap"); // NOLINT
    return Status::SUCCESS;
  } else if ((bits & Status::FAIL) != 0) {               // NOLINT
    ESP_LOGI(LOG_TAG.data(), "Failed to connect to ap"); // NOLINT
    return Status::FAIL;
  }

  ESP_LOGE(LOG_TAG.data(), "UNEXPECTED EVENT"); // NOLINT
  return Status::FAIL;
}

void Wifi::eventHandlerWifi(
  void*            arg, // NOLINT
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            eventData) noexcept { // NOLINT
  if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) {
    ESP_ERROR_CHECK(esp_wifi_connect());
  } else if (
    eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED) {
    // NOLINTNEXTLINE
    ESP_LOGI(LOG_TAG.data(), "Failed to connect to AP, retry: %d", ++s_retries);
    ESP_ERROR_CHECK(esp_wifi_connect());
    xEventGroupClearBits(s_wifiEventGroup, Status::SUCCESS);
    xEventGroupSetBits(s_wifiEventGroup, Status::FAIL);
  }
}

void Wifi::eventHandlerIp(
  void*            arg, // NOLINT
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            eventData) noexcept {
  if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) {
    auto* event = static_cast<ip_event_got_ip_t*>(eventData);
    // NOLINTNEXTLINE
    ESP_LOGI(LOG_TAG.data(), "got ip: " IPSTR, IP2STR(&event->ip_info.ip));
    xEventGroupSetBits(s_wifiEventGroup, Status::SUCCESS);
  }
}

esp_err_t Wifi::tcpCreateSocket(TcpSocket& out) noexcept {
  out = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if (out < 0) {
    // NOLINTNEXTLINE
    ESP_LOGE(LOG_TAG.data(), "Unable to create socket: errno %d", errno);
    return Status::FAIL;
  }
  ESP_LOGI(LOG_TAG.data(), "Socket created"); // NOLINT
  return Status::SUCCESS;
}

esp_err_t Wifi::tcpConnectServer(
  Wifi::TcpSocket& sock, const sockaddr_in* const addr) noexcept {
  TcpErr err = ::connect(
    sock,
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    reinterpret_cast<const sockaddr*>(addr),
    sizeof(*addr));

  WIFI_ERR_CHECK(err); // NOLINT

  ESP_LOGI(LOG_TAG.data(), "Successfully connected"); // NOLINT
  return Status::SUCCESS;
}

esp_err_t Wifi::tcpReceiveData(Wifi::TcpSocket& sock) noexcept {
  esp_err_t err = Status::SUCCESS;

  while (true) {
    DataBuffer rxBuffer{};
    TcpSize    len = recv(sock, rxBuffer.data(), rxBuffer.size() - 1, 0);

    // Error occurred during receiving
    if (len < 0) {
      ESP_LOGE(LOG_TAG.data(), "recv failed: errno %d", errno); // NOLINT
      err = Status::FAIL;
      break;
    }

    // Connection closed
    else if (len == 0) {
      ESP_LOGI(LOG_TAG.data(), "Connection closed"); // NOLINT
      err = Status::SUCCESS;
      break;
    }

    // Data received
    ESP_LOGI(LOG_TAG.data(), "Received %d bytes", len); // NOLINT
    // todo: pass data outside, DON'T BREAK THE LOOP
  }

  WIFI_ERR_CHECK(err); // NOLINT
  return err;
}
