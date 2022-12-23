#include <algorithm>
#include "CommandMotor.h"
#include "pn_logger.h"
#include "pn_macros.h"
#include "Motors.h"
#include "Stepper.h"

esp_err_t CommandMotor::execute(Stepper& motor) const noexcept {
  PN_LOG_INFO(
    "Motor command: %u, %d", static_cast<std::uint8_t>(m_axis), m_steps);

  motor.step(m_steps);
  return Status::SUCCESS;
}

void CommandMotor::registerCommand() const noexcept {
  Motors::getInstance().registerCommand(this);
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

[[nodiscard]] bool CommandMotor::validate(std::string_view raw) noexcept {
  const auto len = raw.length();
  if (
    (len < MIN_LENGTH) ||
    (len == MIN_LENGTH && raw.at(STEPS_SPECIFIER_POS) == '-')) [[unlikely]] {
    PN_LOG_WARN("Command too short");
    return false;
  }

  const auto axis = std::tolower(raw.at(AXIS_SPECIFIER_POS));
  if (axis < 'x' || axis > 'z') [[unlikely]] {
    PN_LOG_WARN("Invalid axis specifier");
    return false;
  }

  if (
    raw.at(STEPS_SPECIFIER_POS) != '-' &&
    !std::isdigit(raw.at(STEPS_SPECIFIER_POS))) [[unlikely]] {
    PN_LOG_WARN("Invalid steps specifier");
    return false;
  }

  if (!std::ranges::all_of(
        raw.substr(STEPS_SPECIFIER_POS + 1), [](const auto chr) noexcept {
          return std::isdigit(chr) || chr == '\0' || chr == '\n' || chr == '\r';
        })) [[unlikely]] {
    PN_LOG_WARN("Steps contain non-digit characters");
    return false;
  }

  return true;
}
