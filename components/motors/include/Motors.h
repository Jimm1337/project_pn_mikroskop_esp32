#ifndef PROJECT_PN_MIKROSKOP_ESP32_MOTORS_H
#define PROJECT_PN_MIKROSKOP_ESP32_MOTORS_H

#include "freertos/FreeRTOS.h"
#include <string_view>
#include <esp_err.h>
#include "CommandMotor.h"
#include "CommandQueue.h"
#include "freertos/semphr.h"
#include "pn_logger.h"
#include <memory>

class Motors final {
private:
  static constexpr std::string_view LOG_TAG{ "Motors" };

private:
  CommandQueue m_commandQueueX;
  CommandQueue m_commandQueueY;
  CommandQueue m_commandQueueZ;

public:
  Motors(const Motors& other)            = delete;
  Motors(Motors&& other)                 = delete;
  Motors& operator=(const Motors& other) = delete;
  Motors& operator=(Motors&& other)      = delete;
  ~Motors() noexcept                     = default;

  template<typename C>
    requires std::is_same_v<CommandMotor, std::remove_cvref_t<C>>
  void registerCommand(C&& command) noexcept;

  static void startTasks() noexcept;

private:
  Motors() noexcept = default;

  [[noreturn]] void taskX(void*) noexcept;
  [[noreturn]] void taskY(void*) noexcept;
  [[noreturn]] void taskZ(void*) noexcept;
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_MOTORS_H