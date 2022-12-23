#include "Stepper.h"
#include "driver/gpio.h"
#include "pn_logger.h"

Stepper::Stepper(
  gpio_num_t         pin1,
  gpio_num_t         pin2,
  gpio_num_t         pin3,
  gpio_num_t         pin4) noexcept:
  m_pin1{ pin1 },
  m_pin2{ pin2 },
  m_pin3{ pin3 },
  m_pin4{ pin4 },
  m_stepCount{ 0 },
  m_stepDelay{ DEFAULT_DELAY } {
  if (initGpio() != Status::SUCCESS) [[unlikely]] {
    PN_LOG_ERROR("Failed to initialize GPIO");
  }
}

void Stepper::step(std::int_fast32_t steps) noexcept {
  if (steps > 0) {
    for (m_stepCount = 0; m_stepCount < steps; m_stepCount++) {
      stepMotor(m_stepCount % 4);
      vTaskDelay(m_stepDelay);
    }
  } else {
    for (m_stepCount = 0; m_stepCount > steps; m_stepCount--) {
      stepMotor(m_stepCount % 4);
      vTaskDelay(m_stepDelay);
    }
  }
}

void Stepper::stepMotor(std::int_fast32_t thisStep) noexcept {
  switch (thisStep) {
  case 0: // 1010
    gpio_set_level(m_pin1, Level::HIGH);
    gpio_set_level(m_pin2, Level::LOW);
    gpio_set_level(m_pin3, Level::HIGH);
    gpio_set_level(m_pin4, Level::LOW);
    break;
  case 1: // 0110
    gpio_set_level(m_pin1, Level::LOW);
    gpio_set_level(m_pin2, Level::HIGH);
    gpio_set_level(m_pin3, Level::HIGH);
    gpio_set_level(m_pin4, Level::LOW);
    break;
  case 2: // 0101
    gpio_set_level(m_pin1, Level::LOW);
    gpio_set_level(m_pin2, Level::HIGH);
    gpio_set_level(m_pin3, Level::LOW);
    gpio_set_level(m_pin4, Level::HIGH);
    break;
  case 3: // 1001
    gpio_set_level(m_pin1, Level::HIGH);
    gpio_set_level(m_pin2, Level::LOW);
    gpio_set_level(m_pin3, Level::LOW);
    gpio_set_level(m_pin4, Level::HIGH);
    break;
  default:
    [[unlikely]] PN_LOG_ERROR("Invalid step number: %d", thisStep);
    break;
  }
}

esp_err_t Stepper::initGpio() const noexcept {
  auto gpioConfig         = GPIO_CONFIG_DEFAULT;
  gpioConfig.pin_bit_mask = getPinMask();

  PN_ERR_CHECK(gpio_config(&gpioConfig));
  PN_ERR_CHECK(gpio_set_level(m_pin1, Level::LOW));
  PN_ERR_CHECK(gpio_set_level(m_pin2, Level::LOW));
  PN_ERR_CHECK(gpio_set_level(m_pin3, Level::LOW));
  PN_ERR_CHECK(gpio_set_level(m_pin4, Level::LOW));

  return Status::SUCCESS;
}
