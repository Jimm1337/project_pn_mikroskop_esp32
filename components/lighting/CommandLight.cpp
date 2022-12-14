#include "CommandLight.h"
#include <charconv>
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

CommandLight::LedNo CommandLight::parseLed(std::string_view raw) noexcept {
  LedNo led = INVALID_LED;

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

void CommandLight::registerCommand() const noexcept {
//  Lighting::getInstance().registerCommand(this);
}