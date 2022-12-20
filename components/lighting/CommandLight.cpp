#include "CommandLight.h"
#include <charconv>
#include <algorithm>
#include "esp_log.h"
#include "pn_logger.h"
#include "pn_macros.h"
#include "Lighting.h"

esp_err_t CommandLight::execute() const noexcept {
  PN_LOG_INFO(
    "Light command: LED:%d, R:%d, G:%d, B:%d, A:%d",
    m_led,
    m_rgba.red,
    m_rgba.green,
    m_rgba.blue,
    m_rgba.alpha);

  // todo: implement light command execution
  PN_UNIMPLEMENTED();
}

void CommandLight::registerCommand() const noexcept {
  // Lighting::getInstance().registerCommand(this);
}

CommandLight::LedNo CommandLight::parseLed(std::string_view raw) noexcept {
  LedNo led = INVALID_LED;

  if (raw.length() < LED_SPECIFIER_POS + 1) {
    PN_LOG_WARN("No LED in command: %s", raw.data());
    return led;
  }

  auto err = std::from_chars(
               raw.begin() + LED_SPECIFIER_POS,
               raw.begin() + LED_SPECIFIER_END,
               led,
               LED_BASE)
               .ec;

  [[unlikely]] if (err != std::errc{}) {
    PN_LOG_INVALID_DATA(
      "Invalid LED specifier: %s", raw.data() + LED_SPECIFIER_POS);
  }

  return led;
}

ColorRGBA CommandLight::parseRGBA(std::string_view raw) noexcept {
  if (raw.length() < RGBA_SPECIFIER_POS + 1) {
    PN_LOG_WARN("No RGBA in command: %s", raw.data());
    return INVALID_COLOR;
  }

  std::uint32_t rawColor{};

  auto err = std::from_chars(
               raw.begin() + RGBA_SPECIFIER_POS,
               raw.begin() + RGBA_SPECIFIER_END,
               rawColor,
               RGBA_BASE)
               .ec;

  [[unlikely]] if (err != std::errc{}) {
    PN_LOG_INVALID_DATA(
      "Invalid RGBA specifier: %s", raw.data() + RGBA_SPECIFIER_POS);
    return INVALID_COLOR;
  }

  return extractColorRGBA(rawColor);
}

bool CommandLight::validate(std::string_view raw) noexcept {
  const auto len = raw.length();
  if (len != MIN_LENGTH) [[unlikely]] {
    PN_LOG_WARN("Command is of wrong length: %d vs %d", len, MIN_LENGTH);
    return false;
  }

  if (!std::ranges::all_of(raw.substr(LED_SPECIFIER_POS, 3), isdigit))
    [[unlikely]] {
    PN_LOG_WARN("LED contains non-digit characters");
    return false;
  }

  if (!std::ranges::all_of(
        raw.substr(RGBA_SPECIFIER_POS), [](const auto chr) noexcept {
          return std::isxdigit(chr) || chr == '\0' || chr == '\n' ||
                 chr == '\r';
        })) [[unlikely]] {
    PN_LOG_WARN("Color contains non-hex characters");
    return false;
  }

  return true;
}
