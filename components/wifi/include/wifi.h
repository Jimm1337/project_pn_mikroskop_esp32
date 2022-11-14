#include <string_view>
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"

namespace Status {
inline constexpr esp_err_t SUCCESS = BIT0;
inline constexpr esp_err_t FAIL    = BIT1;
} // namespace Status

class Wifi final {
  static constexpr std::string_view  LOG_TAG{ "Wifi handler" };
  static constexpr std::uint_fast8_t MAX_RETRIES{ 5 };
  static constexpr std::string_view  SSID{ "ssid" };
  static constexpr std::string_view  PASSWORD{ "password" };
  static constexpr auto AUTH_MODE{ WIFI_AUTH_WPA2_PSK };

private:
  std::uint_fast8_t  m_retries{ 0 };
  EventGroupHandle_t m_wifiEventGroup{};
  wifi_config_t      m_wifiConfig;

public:
  Wifi() noexcept;
  Wifi(const Wifi& other)            = delete;
  Wifi(Wifi&& other)                 = delete;
  Wifi& operator=(const Wifi& other) = delete;
  Wifi& operator=(Wifi&& other)      = delete;
  ~Wifi() noexcept                   = default;

  void connect() noexcept;

private:
  static void connectRegisterHandlers() noexcept;
  void        connectSetup() noexcept;

  esp_err_t connectWaitForConnection() noexcept;

  static void wifiEventHandler(
    void*            arg,
    esp_event_base_t eventBase,
    std::int32_t          eventId,
    void*            eventData) noexcept;

  static void ipEventHandler(
    void*            arg,
    esp_event_base_t eventBase,
    std::int32_t          eventId,
    void*            eventData) noexcept;
};

// todo: tcp
