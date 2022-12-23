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
#include "Stepper.h"

class Motors final {
  static constexpr std::string_view LOG_TAG{ "Motors" };

  static constexpr auto                      STACK_DEPTH    = 4096;
  static constexpr auto                      TASK_PRIORITY  = 1;
  static constexpr std::array<gpio_num_t, 4> STEPPER_X_PINS = {
    GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32, GPIO_NUM_33 // D34, D35, D32, D33
  };
  static constexpr std::array<gpio_num_t, 4> STEPPER_Y_PINS = {
    GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12 // D26, D27, D14, D12
  };
  static constexpr std::array<gpio_num_t, 4> STEPPER_Z_PINS = {
    GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5 // D21, D19, D18, D5
  };

private:
  static inline std::unique_ptr<Motors> s_instance{ nullptr };
  static inline SemaphoreHandle_t       s_semaphore{ nullptr };
  static inline StaticSemaphore_t       s_semaphoreBuffer{};

  CommandQueue<CommandMotor> m_commandQueueX;
  CommandQueue<CommandMotor> m_commandQueueY;
  CommandQueue<CommandMotor> m_commandQueueZ;

  Stepper m_stepperX;
  Stepper m_stepperY;
  Stepper m_stepperZ;

public:
  Motors(const Motors& other)                = delete;
  Motors(Motors&& other) noexcept            = default;
  Motors& operator=(const Motors& other)     = delete;
  Motors& operator=(Motors&& other) noexcept = default;
  ~Motors() noexcept                         = default;

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
  Motors() noexcept;

  [[noreturn]] void taskX() noexcept;
  [[noreturn]] void taskY() noexcept;
  [[noreturn]] void taskZ() noexcept;

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
