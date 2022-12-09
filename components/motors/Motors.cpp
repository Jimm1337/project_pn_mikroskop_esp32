#include "Motors.h"

template<typename C>
  requires std::is_same_v<CommandMotor, std::remove_cvref_t<C>>
void Motors::registerCommand(C&& command) noexcept {
  switch (command.getAxis()) {
  case Axis::X:
    m_commandQueueX.push(std::forward<C>(command));
    break;
  case Axis::Y:
    m_commandQueueY.push(std::forward<C>(command));
    break;
  case Axis::Z:
    m_commandQueueZ.push(std::forward<C>(command));
    break;
  default:
    break;
  }
}

void Motors::startTasks() noexcept {
  //todo
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

