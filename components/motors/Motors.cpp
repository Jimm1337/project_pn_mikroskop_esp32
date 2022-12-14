#include "Motors.h"

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
  default:
    break;
  }
}

void Motors::startTasks() noexcept {
  initMutex();
  getInstance().initQueues();

  xTaskCreate(
    [](void* param) noexcept { getInstance().taskX(param); },
    "MotorsX",
    STACK_DEPTH,
    nullptr,
    1,
    nullptr);
  xTaskCreate(
    [](void* param) noexcept { getInstance().taskY(param); },
    "MotorsY",
    STACK_DEPTH,
    nullptr,
    1,
    nullptr);
  xTaskCreate(
    [](void* param) noexcept { getInstance().taskZ(param); },
    "MotorsZ",
    STACK_DEPTH,
    nullptr,
    1,
    nullptr);
}

[[noreturn]] void Motors::taskX(void* /*pvParameters*/) noexcept {
  while (true) {
    if (m_commandQueueX.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueX.pop();

    command.execute();
  }
}

[[noreturn]] void Motors::taskY(void* /*pvParameters*/) noexcept {
  while (true) {
    if (m_commandQueueY.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueY.pop();

    command.execute();
  }
}

[[noreturn]] void Motors::taskZ(void* /*pvParameters*/) noexcept {
  while (true) {
    if (m_commandQueueZ.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueZ.pop();

    command.execute();
  }
}
