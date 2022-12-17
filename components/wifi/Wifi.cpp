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
    while (true) {
      wifi.startReceiving();
      PN_LOG_WARN("Wifi task stopped receiving");
    }
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
    std::memcpy(cfg.sta.ssid, SSID.data(), SSID.size());             // NOLINT
    std::memcpy(cfg.sta.password, PASSWORD.data(), PASSWORD.size()); // NOLINT
    cfg.sta.threshold.authmode = AUTH_MODE;
    cfg.sta.pmf_cfg.capable    = true;
    cfg.sta.pmf_cfg.required   = false;
    return cfg;
  }() } {
  ESP_ERROR_CHECK(esp_netif_init()); //NOLINT
  esp_netif_create_default_wifi_sta();

  const wifi_init_config_t config = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&config)); //NOLINT

  s_wifiEventGroup = xEventGroupCreate();
}

void Wifi::connect() noexcept {
  if (s_wifiEventGroup == nullptr) [[unlikely]] {
    PN_LOG_ERROR("Wifi event group is null");
    return;
  }

  ESP_ERROR_CHECK(connectRegisterHandlers()); //NOLINT
  ESP_ERROR_CHECK(connectSetup()); //NOLINT
  ESP_ERROR_CHECK(connectWaitForConnection()); //NOLINT

  vEventGroupDelete(s_wifiEventGroup);
  s_wifiEventGroup = nullptr;
}

void Wifi::startReceiving() noexcept {
  const sockaddr_in destAddr = [] {
    sockaddr_in addr{};
    addr.sin_addr.s_addr = inet_addr(SERVER_IP.data());
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(SERVER_PORT);
    return addr;
  }();

  TcpSocket socket{};
  ESP_ERROR_CHECK(tcpCreateSocket(socket)); //NOLINT
  ESP_ERROR_CHECK(tcpConnectServer(socket, &destAddr)); //NOLINT
  ESP_ERROR_CHECK(tcpReceiveData(socket)); // inf loop //NOLINT
}

esp_err_t Wifi::connectRegisterHandlers() noexcept {
  esp_err_t err;

  err = esp_event_handler_instance_register(
    WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi::eventHandlerWifi, nullptr, nullptr);
  PN_ERR_CHECK(err);

  err = esp_event_handler_instance_register(
    IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi::eventHandlerIp, nullptr, nullptr);
  PN_ERR_CHECK(err);

  return err;
}

esp_err_t Wifi::connectSetup() noexcept {
  esp_err_t err;

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
  static constexpr auto waitCondition = WIFI_CONNECTED_BIT | WIFI_FAIL_BIT;

  const EventBits_t bits = xEventGroupWaitBits(
    s_wifiEventGroup, waitCondition, pdFALSE, pdFALSE, portMAX_DELAY);

  if ((bits & WIFI_CONNECTED_BIT) != 0) [[likely]] { // NOLINT
    PN_LOG_INFO("Connected to AP");
    return Status::SUCCESS;

  } else if ((bits & WIFI_FAIL_BIT) != 0) [[unlikely]] { // NOLINT
    PN_LOG_ERROR("Failed to connect to AP");
    return Status::FAIL;
  }

  PN_LOG_UNREACHABLE();
  PN_FAIL();
}

void Wifi::eventHandlerWifi(
  void* /*arg*/,
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            /*eventData*/) noexcept { // NOLINT
  if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START) [[likely]] {
    ESP_ERROR_CHECK(esp_wifi_connect()); //NOLINT

  } else if (
    eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED) {
    PN_LOG_WARN("Disconnected from AP, trying to reconnect...");
    ESP_ERROR_CHECK(esp_wifi_connect()); //NOLINT

    xEventGroupClearBits(s_wifiEventGroup, WIFI_CONNECTED_BIT);
    xEventGroupSetBits(s_wifiEventGroup, WIFI_FAIL_BIT);
  }
}

void Wifi::eventHandlerIp(
  void* /*arg*/,
  esp_event_base_t eventBase,
  std::int32_t     eventId,
  void*            eventData) noexcept {
  if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP) [[likely]] {
    auto* event = static_cast<ip_event_got_ip_t*>(eventData);
    PN_LOG_INFO("Got IP: " IPSTR, IP2STR(&event->ip_info.ip));

    xEventGroupSetBits(s_wifiEventGroup, WIFI_CONNECTED_BIT);
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
  const TcpErr err =
    // NOLINTNEXTLINE
    ::connect(sock, reinterpret_cast<const sockaddr*>(addr), sizeof(*addr));
  PN_ERR_CHECK(err);

  PN_LOG_INFO("Connected to %s:%d", SERVER_IP.data(), SERVER_PORT);
  return Status::SUCCESS;
}

esp_err_t Wifi::tcpReceiveData(TcpSocket& sock) noexcept {
  while (true) {
    DataBuffer    rxBuffer{};
    const TcpSize len = recv(sock, rxBuffer.data(), rxBuffer.size() - 1, 0);

    // Error occurred during receiving
    if (len < 0) [[unlikely]] {
      PN_LOG_WARN("recv failed: errno %d", errno);
      close(sock);
      break;
    }

    // Connection closed
    else if (len == 0) [[unlikely]] {
      PN_LOG_INFO("Connection closed");
      break;
    }

    // Data received
    rxBuffer.at(static_cast<size_t>(len)) = '\0';
    PN_LOG_INFO("Received command (size: %d): %s", len, rxBuffer.data());

    const auto command = Command{ rxBuffer.data() };
    command.registerCommand();
  }

  return Status::SUCCESS;
}
