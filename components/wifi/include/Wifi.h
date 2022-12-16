#ifndef PROJECT_PN_MIKROSKOP_ESP32_WIFI_H
#define PROJECT_PN_MIKROSKOP_ESP32_WIFI_H

#include "pn_config.h"
#include <lwip/sockets.h>
#include <unistd.h>
#include <array>
#include <cstring>
#include <string_view>
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"

class Wifi final {
  static constexpr std::string_view LOG_TAG{ "Wifi handler" };

  static constexpr std::array<std::uint8_t, 32> SSID{ PN_CONFIG_WIFI_SSID };
  static constexpr std::array<std::uint8_t, 64> PASSWORD{ PN_CONFIG_WIFI_PASS };
  static constexpr auto             AUTH_MODE{ PN_CONFIG_WIFI_AUTH };
  static constexpr std::string_view SERVER_IP{ PN_CONFIG_WIFI_ADDR };
  static constexpr std::uint16_t    SERVER_PORT = PN_CONFIG_WIFI_PORT;

  static constexpr auto BUFFER_SIZE   = 64;
  static constexpr auto STACK_SIZE    = 4096;
  static constexpr auto TASK_PRIORITY = 5;

  static constexpr auto WIFI_CONNECTED_BIT = BIT0;
  static constexpr auto WIFI_FAIL_BIT      = BIT1;

  using TcpSocket = int;
  using TcpErr    = int;
  using TcpSize   = ssize_t;

  using DataBuffer = std::array<char, BUFFER_SIZE>;

private:
  static inline EventGroupHandle_t s_wifiEventGroup{};

  wifi_config_t m_wifiConfig;

public:
  static void startTask() noexcept;

  Wifi(const Wifi& other)            = delete;
  Wifi(Wifi&& other)                 = delete;
  Wifi& operator=(const Wifi& other) = delete;
  Wifi& operator=(Wifi&& other)      = delete;
  ~Wifi() noexcept                   = default;

private:
  Wifi() noexcept;

  /**
   * @brief Connects to the wifi network.
   * @note Call first
   */
  void connect() noexcept;

  /**
   * @brief Starts receiving data from the server.
   * @note Call after connect()
   */
  static void startReceiving() noexcept;

  static esp_err_t connectRegisterHandlers() noexcept;
  esp_err_t        connectSetup() noexcept;
  static esp_err_t connectWaitForConnection() noexcept;

  static void eventHandlerWifi(
    void*            arg,
    esp_event_base_t eventBase,
    std::int32_t     eventId,
    void*            eventData) noexcept;
  static void eventHandlerIp(
    void*            arg,
    esp_event_base_t eventBase,
    std::int32_t     eventId,
    void*            eventData) noexcept;

  static esp_err_t tcpCreateSocket(TcpSocket& out) noexcept;
  static esp_err_t tcpConnectServer(
    TcpSocket& sock, const sockaddr_in* const addr) noexcept;
  static esp_err_t tcpReceiveData(TcpSocket& sock) noexcept;
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_WIFI_H
