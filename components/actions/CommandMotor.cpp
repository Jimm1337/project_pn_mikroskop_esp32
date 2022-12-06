#include "CommandMotor.h"
#include "pn_logger.h"
#include "pn_macros.h"

esp_err_t CommandMotor::execute() const noexcept {
  PN_LOG_INFO(
    "Motor command: %u, %d", static_cast<std::uint_fast8_t>(m_axis), m_steps);

  // todo: implement motor command execution
  PN_UNIMPLEMENTED();
}

Axis CommandMotor::parseAxis(std::string_view raw) noexcept {
  switch (std::tolower(raw.at(AXIS_SPECIFIER_POS))) {
  case 'x':
    return Axis::X;
  case 'y':
    return Axis::Y;
  case 'z':
    return Axis::Z;
  [[unlikely]] default:
    PN_LOG_INVALID_DATA(
      "Invalid axis specifier: %c", raw.at(AXIS_SPECIFIER_POS));
    return Axis::INVALID;
  }
}

CommandMotor::Steps CommandMotor::parseSteps(std::string_view raw) noexcept {
  Steps steps = STEPS_INVALID;

  auto err = std::from_chars(
               raw.begin() + STEPS_SPECIFIER_POS, raw.end(), steps, STEPS_BASE)
               .ec;

  if (err != std::errc{}) [[unlikely]] {
    PN_LOG_INVALID_DATA(
      "Invalid steps specifier: %s", raw.data() + STEPS_SPECIFIER_POS);
  }

  return steps;
}
