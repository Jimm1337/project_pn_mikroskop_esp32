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

  class CommandInvalid {
  private:
    uint32_t m_pad1;
    uint32_t m_pad2;

  public:
    static esp_err_t execute() noexcept;
    static void      registerCommand() noexcept;
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

  consteval explicit Command() noexcept: m_command{ CommandInvalid{} } {
  }

  inline void execute() const noexcept {
    std::visit([](auto&& command) { command.execute(); }, m_command);
  }

  [[nodiscard]] inline bool holdsMotorCommand() const noexcept {
    return std::holds_alternative<CommandMotor>(m_command);
  }

  [[nodiscard]] inline bool holdsLightCommand() const noexcept {
    return std::holds_alternative<CommandLight>(m_command);
  }

  [[nodiscard]] inline bool holdsInvalidCommand() const noexcept {
    return std::holds_alternative<CommandInvalid>(m_command);
  }

  inline void registerCommand() const noexcept {
     std::visit([](auto&& command) { command.registerCommand(); }, m_command);
  }

private:
  [[nodiscard]] static inline bool isMotor(
    std::string_view rawCommand) noexcept {
    return std::tolower(rawCommand.at(0)) == 'm';
  }

  [[nodiscard]] static inline bool isLight(
    std::string_view rawCommand) noexcept {
    return std::tolower(rawCommand.at(0)) == 'l';
  }
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_COMMAND_H
