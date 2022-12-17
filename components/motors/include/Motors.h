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
  static constexpr std::string_view LOG_TAG{ "Motors" };

  static constexpr auto STACK_DEPTH   = 4096;
  static constexpr auto TASK_PRIORITY = 1;

private:
  static inline std::unique_ptr<Motors> s_instance{ nullptr };
  static inline SemaphoreHandle_t       s_semaphore{ nullptr };
  static inline StaticSemaphore_t       s_semaphoreBuffer{};

  CommandQueue<CommandMotor> m_commandQueueX;
  CommandQueue<CommandMotor> m_commandQueueY;
  CommandQueue<CommandMotor> m_commandQueueZ;

public:
  Motors(const Motors& other)            = delete;
  Motors(Motors&& other)                 = default;
  Motors& operator=(const Motors& other) = delete;
  Motors& operator=(Motors&& other)      = default;
  ~Motors() noexcept                     = default;

  void        registerCommand(const CommandMotor* command) noexcept;
  static void startTask() noexcept;

  static inline Motors& getInstance() noexcept {
    lock();
    if (s_instance == nullptr) [[unlikely]] {
      s_instance = std::unique_ptr<Motors>(new Motors());
    }
    unlock();

    return *s_instance;
  }

private:
  Motors() noexcept = default;

  [[noreturn]] void taskX(void*) noexcept;
  [[noreturn]] void taskY(void*) noexcept;
  [[noreturn]] void taskZ(void*) noexcept;

  static inline void initMutex() noexcept {
    s_semaphore = xSemaphoreCreateMutexStatic(&s_semaphoreBuffer);
    if (s_semaphore == nullptr) [[unlikely]] {
      PN_LOG_ERROR("Failed to init mutex");
    }
  }

  static inline void lock() noexcept {
    xSemaphoreTake(s_semaphore, portMAX_DELAY);
  }

  static inline void unlock() noexcept {
    xSemaphoreGive(s_semaphore);
  }
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_MOTORS_H
