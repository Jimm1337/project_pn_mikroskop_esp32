#include <string_view>
#include "esp_event.h"
#include "esp_wifi_types.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOS.h"

class Wifi final {
  enum class Status {
    SUCCESS = BIT0,
    FAIL    = BIT1,
  };

  static constexpr std::string_view  LOG_TAG{ "Wifi handler" };
  static constexpr std::uint_fast8_t MAX_RETRIES{ 5 };

private:
  std::uint_fast8_t  m_retries{ 0 };
  EventGroupHandle_t m_wifiEventGroup{};
  wifi_config_t      m_wifiConfig{ .sta = {
                                     .ssid     = "ssid",
                                     .password = "password",
                                     .threshold = { .authmode = WIFI_AUTH_WPA2_PSK },
                                     .pmf_cfg = { .capable  = true,
                                                  .required = false },
                              } };

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
  void connectSetup() noexcept;

  Status connectWaitForConnection() noexcept;

  static void wifiEventHandler(
    void*            arg,
    esp_event_base_t eventBase,
    int32_t          eventId,
    void*            eventData) noexcept;

  static void ipEventHandler(
    void*            arg,
    esp_event_base_t eventBase,
    int32_t          eventId,
    void*            eventData) noexcept;
};

//todo: tcp
