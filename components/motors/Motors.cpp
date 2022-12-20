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

  [[unlikely]] default:
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
    command.execute();
  }
}

[[noreturn]] void Motors::taskY() noexcept {
  while (true) {
    if (m_commandQueueY.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueY.pop();
    command.execute();
  }
}

[[noreturn]] void Motors::taskZ() noexcept {
  while (true) {
    if (m_commandQueueZ.isEmpty()) {
      vTaskDelay(1);
      continue;
    }

    auto command = m_commandQueueZ.pop();
    command.execute();
  }
}
