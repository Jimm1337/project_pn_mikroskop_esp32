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
  //TODO: SET THIS TO YOUR OWN WIFI CREDENTIALS

  static constexpr std::string_view  LOG_TAG{ "Wifi handler" };
  static constexpr std::uint_fast8_t MAX_RETRIES{ 5 };
  static constexpr std::uint_fast8_t BUFFER_SIZE{ 64 };

  static constexpr std::string_view SSID{ "ssid" };
  static constexpr std::string_view PASSWORD{ "password" };
  static constexpr auto             AUTH_MODE{ WIFI_AUTH_WPA2_PSK };

  static constexpr std::string_view   SERVER_IP{};
  static constexpr std::uint_fast16_t SERVER_PORT{ 0x0 };

  using TcpSocket = int;
  using TcpErr    = int;
  using TcpSize   = ssize_t;

  using DataBuffer = std::array<char, BUFFER_SIZE>;

private:
  static inline std::uint_fast8_t  s_retries{ 0 };
  static inline EventGroupHandle_t s_wifiEventGroup{};
  wifi_config_t                    m_wifiConfig{};

public:
  Wifi() noexcept;
  Wifi(const Wifi& other)            = delete;
  Wifi(Wifi&& other)                 = delete;
  Wifi& operator=(const Wifi& other) = delete;
  Wifi& operator=(Wifi&& other)      = delete;
  ~Wifi() noexcept                   = default;

  /**
   * @brief Connects to the wifi network.
   * @note Call first
   */
  void connect() noexcept;

  /**
   * @brief Starts receiving data from the server.
   * @note Call after connect()
   */
  void startReceiving() noexcept;

private:
  esp_err_t connectRegisterHandlers() noexcept;
  esp_err_t connectSetup() noexcept;
  esp_err_t connectWaitForConnection() noexcept;

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
