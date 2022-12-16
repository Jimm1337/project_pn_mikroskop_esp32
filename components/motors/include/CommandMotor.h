#ifndef PROJECT_PN_MIKROSKOP_ESP32_COMMANDMOTOR_H
#define PROJECT_PN_MIKROSKOP_ESP32_COMMANDMOTOR_H

#include <esp_err.h>
#include <charconv>
#include <string_view>
#include "esp_log.h"
#include "pn_logger.h"

enum class Axis : std::int_fast8_t {
  X,
  Y,
  Z,
  INVALID
};

/**
 * @brief Command to move a motor.
 * @note The command is in the form of "m(AXIS)(STEPS)". For example,
 * "motorX1000" moves the X axis 1000 steps.
 * @note AXIS is a single character, either X, Y or Z.
 * @note STEPS is a number, either in decimal or hexadecimal.
 */
class CommandMotor final {
  static constexpr std::string_view LOG_TAG{ "CommandMotor" };

  static constexpr auto AXIS_SPECIFIER_POS  = 1; // len = 1
  static constexpr auto STEPS_SPECIFIER_POS = 2; // len = ?

  static constexpr auto STEPS_BASE = 10;

  using Steps = std::int_fast16_t;

  static constexpr Steps STEPS_INVALID = 0;

  static constexpr auto MIN_LENGTH = 4;

private:
  Axis  m_axis;
  Steps m_steps;

public:
  CommandMotor(const CommandMotor& other)            = default;
  CommandMotor(CommandMotor&& other)                 = default;
  CommandMotor& operator=(const CommandMotor& other) = default;
  CommandMotor& operator=(CommandMotor&& other)      = default;
  ~CommandMotor() noexcept                           = default;

  inline explicit CommandMotor(std::string_view raw) noexcept:
    m_axis{ Axis::INVALID },
    m_steps{ STEPS_INVALID } {
    if (validate(raw)) {
      m_axis  = parseAxis(raw);
      m_steps = parseSteps(raw);
    }
  }

  constexpr CommandMotor() noexcept:
    m_axis{ Axis::INVALID },
    m_steps{ STEPS_INVALID } {
  }

  esp_err_t execute() const noexcept; // NOLINT

  [[nodiscard]] inline Axis getAxis() const noexcept {
    return m_axis;
  }

  [[nodiscard]] inline Steps getSteps() const noexcept {
    return m_steps;
  }

  void registerCommand() const noexcept;

private:
  [[nodiscard]] static Axis  parseAxis(std::string_view raw) noexcept;
  [[nodiscard]] static Steps parseSteps(std::string_view raw) noexcept;

  [[nodiscard]] static bool validate(std::string_view raw) noexcept;
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_COMMANDMOTOR_H
