#ifndef PROJECT_PN_MIKROSKOP_ESP32_COMMAND_H
#define PROJECT_PN_MIKROSKOP_ESP32_COMMAND_H

#include <string_view>
#include <variant>
#include "CommandLight.h"
#include "CommandMotor.h"
#include "esp_log.h"
#include "pn_macros.h"

/**
 * @brief Command class
 * @note This class is a wrapper for the actual commands. It is used to
 * determine which command is being executed.
 */
class Command final {
  static constexpr std::string_view LOG_TAG{ "Command" };
  static constexpr auto             COMMAND_SPECIFIER_LENGTH = 5;

  struct CommandInvalid {
    static esp_err_t execute() noexcept;

  private:
    uint32_t m_pad1;
    uint32_t m_pad2;
  };

  using UnionCommands =
    std::variant<CommandMotor, CommandLight, CommandInvalid>;

private:
  UnionCommands m_command;

public:
  Command(const Command& other)            = default;
  Command(Command&& other)                 = default;
  Command& operator=(const Command& other) = default;
  Command& operator=(Command&& other)      = default;
  ~Command() noexcept                      = default;

  explicit Command(std::string_view rawCommand) noexcept;

  inline void execute() const noexcept {
    std::visit([](auto&& command) { command.execute(); }, m_command);
  }

private:
  [[nodiscard]] static inline bool isMotorCommand(
    std::string_view rawCommand) noexcept {
    return std::tolower(rawCommand.at(0)) == 'm';
  }

  [[nodiscard]] static inline bool isLightCommand(
    std::string_view rawCommand) noexcept {
    return std::tolower(rawCommand.at(0)) == 'l';
  }
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_COMMAND_H
