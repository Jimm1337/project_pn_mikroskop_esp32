#include "Wifi.h"
#include <cstring>
#include <string>
#include "Command.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "freertos/task.h"
#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "pn_logger.h"
#include "pn_macros.h"

/**
 * @brief Start the wifi task.
 */
void Wifi::startTask() noexcept {
  xTaskCreate(
    [](void*) {
    Wifi wifi{};
    wifi.connect();
    wifi.startReceiving();
    },
    "wifi",
    STACK_SIZE,
    nullptr,
    TASK_PRIORITY,
    nullptr);
}

Wifi::Wifi() noexcept:
  m_wifiConfig{ [] {
    wifi_config_t cfg{};
    std::strncpy(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<char*>(cfg.sta.ssid),
      SSID.data(),
      SSID.size() + 1);
    std::strncpy(
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
      reinterpret_cast<char*>(cfg.sta.password),
      PASSWORD.data(),
      PASSWORD.size() + 1);
    cfg.sta.threshold.authmode = AUTH_MODE;
    cfg.sta.pmf_cfg.capable    = true;
    cfg.sta.pmf_cfg.required   = false;
    return cfg;
  }() } {
  ESP_ERROR_CHECK(esp_netif_init());
  esp_netif_create_default_wifi_sta();
  wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&config));
  s_wifiEventGroup = xEventGroupCreate();
}

void Wifi::connect() noexcept {
  if (s_wifiEventGroup == nullptr) [[unlikely]] {
    PN_LOG_ERROR("Wifi event group is null");
    return;
  }

  ESP_ERROR_CHECK(connectRegisterHandlers());
  ESP_ERROR_CHECK(connectSetup());
  ESP_ERROR_CHECK(connectWaitForConnection());

  vEventGroupDelete(s_wifiEventGroup);
}

void Wifi::startReceiving() noexcept {
  sockaddr_in destAddr = [] {
    sockaddr_in addr{};
    addr.sin_addr.s_addr = inet_addr(SERVER_IP.data());
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(SERVER_PORT);
    return addr;
  }();

  TcpSocket socket{};
  ESP_ERROR_CHECK(tcpCreateSocket(socket));
  ESP_ERROR_CHECK(tcpConnectServer(socket, &destAddr));
  ESP_ERROR_CHECK(tcpReceiveData(socket)); // inf loop
}

esp_err_t Wifi::connectRegisterHandlers() noexcept {
  esp_err_t err{};

  err = esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi::eventHandlerWifi, nullptr, nullptr);
  PN_ERR_CHECK(err);

  err = esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi::eventHandlerIp, nullptr, nullptr);
  PN_ERR_CHECK(err);

  return err;
}

esp_err_t Wifi::connectSetup() noexcept {
  esp_err_t err{};

  err = esp_wifi_set_mode(WIFI_MODE_STA);
  PN_ERR_CHECK(err);

  err = esp_wifi_set_config(WIFI_IF_STA, &m_wifiConfig);
  PN_ERR_CHECK(err);

  err = esp_wifi_start();
  PN_ERR_CHECK(err);

  PN_LOG_INFO("Wifi setup complete");

  return err;
}

esp_err_t Wifi::connectWaitForConnection() noexcept {
  // NOLINTNEXTLINE
  static constexpr EventBits_t waitCondition = Status::SUCCESS | Status::FAIL;

  const EventBits_t bits = xEventGroupWaitBits(
    s_wifiEventGroup, waitCondition, pdFALSE, pdFALSE, portMAX_DELAY);

  if ((bits & Status::SUCCESS) != 0) [[likely]] { // NOLINT
    PN_LOG_INFO("Connected to AP");
    return Status::SUCCESS;
  } else if ((bits & Status::FAIL) != 0) [[unlikely]] { // NOLINT
    PN_LOG_WARN("Failed to connect to AP");
    return Status::FAIL;
  }

  PN_FAIL(); // unreachable
}

void Wifi::eventHandlerWifi(
  void*            arg, // NOLINT
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            eventData) noexcept { // NOLINT
  if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) [[likely]] {
    ESP_ERROR_CHECK(esp_wifi_connect());
  } else if (
    eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED) {
    PN_LOG_WARN("Disconnected from AP, trying to reconnect...");
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
  if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) [[likely]] {
    auto* event = static_cast<ip_event_got_ip_t*>(eventData);
    PN_LOG_INFO("Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

    xEventGroupSetBits(s_wifiEventGroup, Status::SUCCESS);
  }
}

esp_err_t Wifi::tcpCreateSocket(TcpSocket& out) noexcept {
  out = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

  if (out < 0) [[unlikely]] {
    PN_LOG_ERROR("Unable to create socket: %d", errno);
    return Status::FAIL;
  }

  PN_LOG_INFO("Socket created");

  return Status::SUCCESS;
}

esp_err_t Wifi::tcpConnectServer(
  TcpSocket& sock, const sockaddr_in* addr) noexcept {
  TcpErr err =
    // NOLINTNEXTLINE
    ::connect(sock, reinterpret_cast<const sockaddr*>(addr), sizeof(*addr));
  PN_ERR_CHECK(err);

  PN_LOG_INFO("Connected to %s:%d", SERVER_IP.data(), SERVER_PORT);

  return Status::SUCCESS;
}

esp_err_t Wifi::tcpReceiveData(TcpSocket& sock) noexcept {
  esp_err_t err = Status::SUCCESS;

  while (true) {
    DataBuffer rxBuffer{};
    TcpSize    len = recv(sock, rxBuffer.data(), rxBuffer.size() - 1, 0);

    // Error occurred during receiving
    if (len < 0) [[unlikely]] {
      PN_LOG_WARN("recv failed: errno %d", errno);
      err = Status::FAIL;
    }

    // Connection closed
    else if (len == 0) [[unlikely]] {
      PN_LOG_INFO("Connection closed");
      err = Status::SUCCESS;
      break;
    }

    // Data received
    PN_LOG_INFO("Received %d bytes", len);
    // todo: pass data outside, DON'T BREAK THE LOOP

    PN_ERR_CHECK(err); // NOLINT
  }

  return err;
}
