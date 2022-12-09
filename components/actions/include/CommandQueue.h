#ifndef PROJECT_PN_MIKROSKOP_ESP32_COMMANDQUEUE_H
#define PROJECT_PN_MIKROSKOP_ESP32_COMMANDQUEUE_H

#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "Command.h"
#include "../../main/include/pn_logger.h"

class CommandQueue final {
  static constexpr std::string_view LOG_TAG{ "CommandQueue" };
  static constexpr auto             QUEUE_LENGTH = 10;
  static constexpr auto QUEUE_TIME_TO_WAIT       = 1000 / portTICK_PERIOD_MS;

private:
  QueueHandle_t m_queueHandle;

public:
  CommandQueue(const CommandQueue& other)            = delete;
  CommandQueue(CommandQueue&& other)                 = delete;
  CommandQueue& operator=(const CommandQueue& other) = delete;
  CommandQueue& operator=(CommandQueue&& other)      = delete;

  inline CommandQueue() noexcept:
    m_queueHandle(xQueueCreate(QUEUE_LENGTH, sizeof(Command))) {
    if (m_queueHandle == nullptr) [[unlikely]] {
      PN_LOG_ERROR("Failed to create queue");
    }
  }

  inline ~CommandQueue() noexcept {
    vQueueDelete(m_queueHandle);
  }

  template<typename... Args>
  inline void push(Args&&... commands) noexcept {
    if (
      xQueueGenericSend(
        m_queueHandle,
        &std::forward<Args>(commands)...,
        QUEUE_TIME_TO_WAIT,
        queueSEND_TO_BACK) == pdTRUE) [[likely]] {
      PN_LOG_DEBUG("Pushed command to queue");
    } else {
      PN_LOG_ERROR("Failed to push command to queue");
    }
  }

  [[nodiscard]] inline Command pop() noexcept {
    Command buffer{};

    if (xQueueReceive(m_queueHandle, &buffer, QUEUE_TIME_TO_WAIT) != pdFALSE)
      [[likely]] {
      PN_LOG_DEBUG("Popped command from queue");
    } else {
      PN_LOG_ERROR("Failed to pop command from queue");
    }

    return buffer;
  }

  [[nodiscard]] inline bool isEmpty() const noexcept {
    return uxQueueMessagesWaiting(m_queueHandle) == 0;
  }

  [[nodiscard]] inline bool isFull() const noexcept {
    return uxQueueMessagesWaiting(m_queueHandle) == QUEUE_LENGTH;
  }

  inline void clear() noexcept {
    xQueueReset(m_queueHandle);
  }
};

#endif // PROJECT_PN_MIKROSKOP_ESP32_COMMANDQUEUE_H
