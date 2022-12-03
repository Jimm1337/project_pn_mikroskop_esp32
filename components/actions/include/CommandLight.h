#ifndef PROJECT_PN_MIKROSKOP_ESP32_COMMANDLIGHT_H
#define PROJECT_PN_MIKROSKOP_ESP32_COMMANDLIGHT_H

#include <esp_err.h>
#include <limits>
#include <string_view>

struct ColorRGBA {
  std::uint8_t red;
  std::uint8_t green;
  std::uint8_t blue;
  std::uint8_t alpha;
};

/**
 * @brief Command to set the color of a led
 * @note The command is in the format: "l(LED)(R)(G)(B)(A)", for example:
 * "l001ff0000" sets the color of led 1 to red.
 * @note LED is a three digit number in base 10
 * @note R, G, B and A are two digit numbers in base 16
 */
class CommandLight final {
  static constexpr std::string_view LOG_TAG{ "CommandLight" };

  static constexpr auto LED_SPECIFIER_POS  = 1; // len = 3
  static constexpr auto LED_SPECIFIER_END  = 4;
  static constexpr auto RGBA_SPECIFIER_POS = 4; // len = 8
  static constexpr auto RGBA_SPECIFIER_END = 12;

  static constexpr auto LED_BASE  = 10;
  static constexpr auto RGBA_BASE = 16;

  using LedNo = std::uint_fast16_t;

  static constexpr LedNo     INVALID_LED   = std::numeric_limits<LedNo>::max();
  static constexpr ColorRGBA INVALID_COLOR = { 0, 0, 0, 0 };

private:
  LedNo     m_led;
  ColorRGBA m_rgba;

public:
  CommandLight(const CommandLight& other)            = default;
  CommandLight(CommandLight&& other)                 = default;
  CommandLight& operator=(const CommandLight& other) = default;
  CommandLight& operator=(CommandLight&& other)      = default;
  ~CommandLight() noexcept                           = default;

  explicit CommandLight(std::string_view raw) noexcept;

  esp_err_t execute() const noexcept; // NOLINT

  [[nodiscard]] inline LedNo getLed() const noexcept {
    return m_led;
  }

  [[nodiscard]] inline ColorRGBA getRGBA() const noexcept {
    return m_rgba;
  }

private:
  [[nodiscard]] static LedNo     parseLed(std::string_view raw) noexcept;
  [[nodiscard]] static ColorRGBA parseRGBA(std::string_view raw) noexcept;

  // extract colors with bitmasks
  [[nodiscard]] static inline ColorRGBA extractColorRGBA(
    std::uint32_t raw) noexcept {
    return { static_cast<std::uint8_t>((raw >> 24U) & 0xFFU), // NOLINT
             static_cast<std::uint8_t>((raw >> 16U) & 0xFFU), // NOLINT
             static_cast<std::uint8_t>((raw >> 8U) & 0xFFU),  // NOLINT
             static_cast<std::uint8_t>(raw & 0xFFU) };        // NOLINT
  }
};
#endif // PROJECT_PN_MIKROSKOP_ESP32_COMMANDLIGHT_H
