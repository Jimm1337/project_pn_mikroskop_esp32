#ifndef PROJECT_PN_MIKROSKOP_ESP32_COMMANDQUEUE_H
#define PROJECT_PN_MIKROSKOP_ESP32_COMMANDQUEUE_H

#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include "pn_logger.h"
#include "Command.h"

template<typename T>
concept validCMD = requires(T cmd) {
                     { cmd.registerCommand() } -> std::same_as<void>;
                   };

template<validCMD CommandType>
class CommandQueue final {
  static constexpr std::string_view LOG_TAG{ "CommandQueue" };

  static constexpr auto QUEUE_LENGTH       = 10;
  static constexpr auto QUEUE_TIME_TO_WAIT = 1000 / portTICK_PERIOD_MS;

private:
  QueueHandle_t m_queueHandle;

public:
  inline CommandQueue() noexcept:
    m_queueHandle{ xQueueCreate(QUEUE_LENGTH, sizeof(CommandType)) } {
    if (m_queueHandle == nullptr) [[unlikely]] {
      PN_LOG_ERROR("Failed to create queue");
    }
  }

  inline ~CommandQueue() noexcept {
    vQueueDelete(m_queueHandle);
  }

  CommandQueue(const CommandQueue& other)            = delete;
  CommandQueue& operator=(const CommandQueue& other) = delete;

  inline CommandQueue(CommandQueue&& other) noexcept:
    m_queueHandle(other.m_queueHandle) {
    other.m_queueHandle = nullptr;
  }

  inline CommandQueue& operator=(CommandQueue&& other) noexcept {
    if (this != &other) {
      m_queueHandle       = other.m_queueHandle;
      other.m_queueHandle = nullptr;
    }
    return *this;
  }

  inline void push(const CommandType* command) noexcept {
    if (
      xQueueGenericSend(
        m_queueHandle, command, QUEUE_TIME_TO_WAIT, queueSEND_TO_BACK) ==
      pdTRUE) [[likely]] {
      PN_LOG_DEBUG("Pushed command to queue");
    } else {
      PN_LOG_ERROR("Failed to push command to queue");
    }
  }

  [[nodiscard]] inline CommandType pop() noexcept {
    CommandType buffer{};

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
