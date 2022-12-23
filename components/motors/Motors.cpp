#include "Motors.h"

Motors::Motors() noexcept:
  m_stepperX{ STEPPER_X_PINS.at(0),
              STEPPER_X_PINS.at(1),
              STEPPER_X_PINS.at(2),
              STEPPER_X_PINS.at(3) },
  m_stepperY{ STEPPER_X_PINS.at(0),
              STEPPER_X_PINS.at(1),
              STEPPER_X_PINS.at(2),
              STEPPER_X_PINS.at(3) },
  m_stepperZ{ STEPPER_Z_PINS.at(0),
              STEPPER_Z_PINS.at(1),
              STEPPER_Z_PINS.at(2),
              STEPPER_Z_PINS.at(3) } {
  PN_LOG_DEBUG("Created Motors");
}


void Motors::registerCommand(const CommandMotor* command) noexcept {
  switch (command->getAxis()) {
  case Axis::X:
    m_commandQueueX.push(command);
    break;

  case Axis::Y:
    m_commandQueueY.push(command);
    break;

  case Axis::Z:
    m_commandQueueZ.push(command);
    break;

  default: [[unlikely]]
    break;
  }
}

void Motors::startTask() noexcept {
  initMutex();

  xTaskCreate(
    [](void*) noexcept { getInstance().taskX(); },
    "MotorsX",
    STACK_DEPTH,
    nullptr,
    TASK_PRIORITY,
    nullptr);

  xTaskCreate(
    [](void*) noexcept { getInstance().taskY(); },
    "MotorsY",
    STACK_DEPTH,
    nullptr,
    TASK_PRIORITY,
    nullptr);

  xTaskCreate(
    [](void*) noexcept { getInstance().taskZ(); },
    "MotorsZ",
    STACK_DEPTH,
    nullptr,
    TASK_PRIORITY,
    nullptr);

  PN_LOG_INFO("Started Motors tasks");
}

[[noreturn]] void Motors::taskX() noexcept {
  while (true) {
    if (m_commandQueueX.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueX.pop();
    command.execute(m_stepperX);
  }
}

[[noreturn]] void Motors::taskY() noexcept {
  while (true) {
    if (m_commandQueueY.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueY.pop();
    command.execute(m_stepperY);
  }
}

[[noreturn]] void Motors::taskZ() noexcept {
  while (true) {
    if (m_commandQueueZ.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueZ.pop();
    command.execute(m_stepperZ);
  }
}
