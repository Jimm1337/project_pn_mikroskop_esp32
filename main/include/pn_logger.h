#ifndef PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H
#define PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H

#include "../pn_config.h"

#if DEBUG == 1

#include <source_location>
#include "esp_log.h"

namespace pn::log {

template<typename... Args>
inline void info(const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGI( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_INFO, tag, format, std::forward<Args>(args)...);
}

template<>
inline void info(const char* tag, const char* format) noexcept {
  ESP_LOGI( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_INFO, tag, "%s", format); // NOLINT
}

inline void info(const char* tag) noexcept {
  ESP_LOGI( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
}

template<typename... Args>
inline void warn(const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGW( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_WARN, tag, format, std::forward<Args>(args)...);
}

template<>
inline void warn(const char* tag, const char* format) noexcept {
  ESP_LOGW( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_WARN, tag, "%s", format); // NOLINT
}

inline void warn(const char* tag) noexcept {
  ESP_LOGW( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
}

template<typename... Args>
inline void error(
  const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGE( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_ERROR, tag, format, std::forward<Args>(args)...);
}

template<>
inline void error(const char* tag, const char* format) noexcept {
  ESP_LOGE( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_ERROR, tag, "%s", format); // NOLINT
}

inline void error(const char* tag) noexcept {
  ESP_LOGE( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
}

template<typename... Args>
inline void debug(
  const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGD( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_DEBUG, tag, format, std::forward<Args>(args)...);
}

template<>
inline void debug(const char* tag, const char* format) noexcept {
  ESP_LOGD( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_DEBUG, tag, "%s", format); // NOLINT
}

inline void debug(const char* tag) noexcept {
  ESP_LOGD( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
}

template<typename... Args>
inline void verbose(
  const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGV( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_VERBOSE, tag, format, std::forward<Args>(args)...);
}

template<>
inline void verbose(const char* tag, const char* format) noexcept {
  ESP_LOGV(tag, "INVALID DATA"); // NOLINT
  ESP_LOGV(                      // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_VERBOSE, tag, "%s", format); // NOLINT
}

inline void verbose(const char* tag) noexcept {
  ESP_LOGV( // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
}

template<typename... Args>
inline void invalidData(
  const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGW(tag, "Invalid data"); // NOLINT
  ESP_LOGW(                      // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_WARN, tag, format, std::forward<Args>(args)...);
}

template<>
inline void invalidData(const char* tag, const char* format) noexcept {
  ESP_LOGW(tag, "Invalid data"); // NOLINT
  ESP_LOGW(                      // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
  esp_log_write(ESP_LOG_WARN, tag, "%s", format); // NOLINT
}

inline void invalidData(const char* tag) noexcept {
  ESP_LOGW(tag, "Invalid data"); // NOLINT
  ESP_LOGW(                      // NOLINT
    tag,
    "Location: %s",
    std::source_location::current().function_name());
}

} // namespace pn::log

#else // DEBUG == 0

namespace pn::log {

template<typename... Args>
inline void info(const char* tag, const char* format, Args&&... args) noexcept {
}

inline void info(const char* tag) noexcept {
}

template<typename... Args>
inline void warn(const char* tag, const char* format, Args&&... args) noexcept {
}

inline void warn(const char* tag) noexcept {
}

template<typename... Args>
inline void error(
  const char* tag, const char* format, Args&&... args) noexcept {
}

inline void error(const char* tag) noexcept {
}

template<typename... Args>
inline void debug(
  const char* tag, const char* format, Args&&... args) noexcept {
}

inline void debug(const char* tag) noexcept {
}

template<typename... Args>
inline void verbose(
  const char* tag, const char* format, Args&&... args) noexcept {
}

inline void verbose(const char* tag) noexcept {
}

template<typename... Args>
inline void invalidData(
  const char* tag, const char* format, Args&&... args) noexcept {
}

inline void invalidData(const char* tag) noexcept {
}

} // namespace pn::log

#endif // DEBUG

#endif // PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H
