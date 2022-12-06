#include "Command.h"
#include "esp_log.h"
#include "pn_logger.h"
#include "pn_macros.h"

esp_err_t Command::CommandInvalid::execute() noexcept {
  PN_FAIL(); // NOLINT
}

Command::Command(std::string_view rawCommand) noexcept:
  m_command{ [rawCommand] {
    if (isMotorCommand(rawCommand)) {
      return UnionCommands{ CommandMotor{ rawCommand } };
    } else if (isLightCommand(rawCommand)) {
      return UnionCommands{ CommandLight{ rawCommand } };
    } else [[unlikely]] {
      PN_LOG_INVALID_DATA("Invalid command: %s", rawCommand.data());
      return UnionCommands{ CommandInvalid{} };
    }
  }() } {
}
