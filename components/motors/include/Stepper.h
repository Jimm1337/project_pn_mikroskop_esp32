#ifndef PROJECT_PN_MIKROSKOP_ESP32_STEPPER_H
#define PROJECT_PN_MIKROSKOP_ESP32_STEPPER_H

/**
 * @file Stepper.h
 * Inspiration from Arduino's Stepper class.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <hal/gpio_types.h>
#include <string_view>
#include <driver/gpio.h>

namespace Level {
inline constexpr std::uint32_t LOW  = 0;
inline constexpr std::uint32_t HIGH = 1;
} // namespace Level

class Stepper final {
  static constexpr std::string_view LOG_TAG{ "Stepper" };
  static constexpr auto             DEFAULT_DELAY = 1 / portTICK_PERIOD_MS;
  static constexpr auto             STEPS_PER_REVOLUTION = 200;
  static constexpr gpio_config_t    GPIO_CONFIG_DEFAULT{
       .pin_bit_mask = 0,
       .mode         = GPIO_MODE_OUTPUT,
       .pull_up_en   = GPIO_PULLUP_DISABLE,
       .pull_down_en = GPIO_PULLDOWN_DISABLE,
       .intr_type    = GPIO_INTR_DISABLE,
  };

private:
  gpio_num_t         m_pin1;
  gpio_num_t         m_pin2;
  gpio_num_t         m_pin3;
  gpio_num_t         m_pin4;
  std::int_fast32_t  m_stepCount;
  TickType_t         m_stepDelay;

public:
  Stepper(
    gpio_num_t pin1,
    gpio_num_t pin2,
    gpio_num_t pin3,
    gpio_num_t pin4) noexcept;

  void step(std::int_fast32_t steps) noexcept;

  inline void setSpeed(std::uint_fast32_t rpm) noexcept {
    static constexpr auto MS_TO_MIN = 60UL * 1000UL / portTICK_PERIOD_MS;

    m_stepDelay = MS_TO_MIN / STEPS_PER_REVOLUTION / rpm;
  }

private:
  void stepMotor(std::int_fast32_t thisStep) noexcept;

  [[nodiscard]] constexpr std::uint64_t getPinMask() const noexcept {
    return (1ULL << m_pin1) | (1ULL << m_pin2) | (1ULL << m_pin3) |
           (1ULL << m_pin4);
  }

  [[nodiscard]] esp_err_t initGpio() const noexcept;
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_STEPPER_H
