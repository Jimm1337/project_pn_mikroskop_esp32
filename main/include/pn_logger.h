#ifndef PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H
#define PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H

#include "../pn_config.h"
#include "pn_macros.h"
#include <source_location>

#if DEBUG == 1
#include "esp_log.h"

namespace pn::internal::log {

template<typename... Args>
inline void info(const char* tag, const char* format, Args&&... args) noexcept {
  esp_log_write(
    ESP_LOG_INFO,
    tag,
    format,
    esp_log_timestamp(),
    tag,
    std::forward<Args>(args)...);
}

template<>
inline void info(const char* tag, const char* format) noexcept {
  esp_log_write(ESP_LOG_INFO, tag, format, esp_log_timestamp(), tag);
}

template<typename... Args>
inline void warn(const char* tag, const char* format, Args&&... args) noexcept {
  esp_log_write(
    ESP_LOG_WARN,
    tag,
    format,
    esp_log_timestamp(),
    tag,
    std::forward<Args>(args)...);
}

template<>
inline void warn(const char* tag, const char* format) noexcept {
  esp_log_write(ESP_LOG_WARN, tag, format, esp_log_timestamp(), tag);
}

template<typename... Args>
inline void error(
  const char* tag, const char* format, Args&&... args) noexcept {
  esp_log_write(
    ESP_LOG_ERROR,
    tag,
    format,
    esp_log_timestamp(),
    tag,
    std::forward<Args>(args)...);
}

template<>
inline void error(const char* tag, const char* format) noexcept {
  esp_log_write(ESP_LOG_ERROR, tag, format, esp_log_timestamp(), tag);
}

template<typename... Args>
inline void debug(
  const char* tag, const char* format, Args&&... args) noexcept {
  esp_log_write(
    ESP_LOG_DEBUG,
    tag,
    format,
    esp_log_timestamp(),
    tag,
    std::forward<Args>(args)...);
}

template<>
inline void debug(const char* tag, const char* format) noexcept {
  esp_log_write(ESP_LOG_DEBUG, tag, format, esp_log_timestamp(), tag);
}

template<typename... Args>
inline void verbose(
  const char* tag, const char* format, Args&&... args) noexcept {
  esp_log_write(
    ESP_LOG_VERBOSE,
    tag,
    format,
    esp_log_timestamp(),
    tag,
    std::forward<Args>(args)...);
}

template<>
inline void verbose(const char* tag, const char* format) noexcept {
  esp_log_write(ESP_LOG_VERBOSE, tag, format, esp_log_timestamp(), tag);
}

template<typename... Args>
inline void invalidData(
  const char* tag, const char* format, Args&&... args) noexcept {
  ESP_LOGW(tag, "Invalid data"); // NOLINT
  esp_log_write(
    ESP_LOG_WARN,
    tag,
    format,
    esp_log_timestamp(),
    tag,
    std::forward<Args>(args)...);
}

template<>
inline void invalidData(const char* tag, const char* format) noexcept {
  ESP_LOGW(tag, "Invalid data"); // NOLINT
  esp_log_write(ESP_LOG_WARN, tag, format, esp_log_timestamp(), tag);
}

inline void location(
  const char*                tag,
  const std::source_location location =
    std::source_location::current()) noexcept {
  ESP_LOGI(
    tag,
    "Location: %s:%lu in %s ",
    location.file_name(),
    location.line(),
    location.function_name()); // NOLINT
}

} // namespace pn::internal::log

#else // DEBUG == 0

namespace pn::internal::log {

template<typename... Args>
inline void info(const char* tag, const char* format, Args&&... args) noexcept {
}

template<typename... Args>
inline void warn(const char* tag, const char* format, Args&&... args) noexcept {
}

template<typename... Args>
inline void error(
  const char* tag, const char* format, Args&&... args) noexcept {
}

template<typename... Args>
inline void debug(
  const char* tag, const char* format, Args&&... args) noexcept {
}

template<typename... Args>
inline void verbose(
  const char* tag, const char* format, Args&&... args) noexcept {
}

template<typename... Args>
inline void invalidData(
  const char* tag, const char* format, Args&&... args) noexcept {
}

inline void location(
  const char*                tag,
  const std::source_location location =
    std::source_location::current()) noexcept {
}

} // namespace pn::log

#endif // DEBUG

#define PN_LOG_INFO(format, ...)                                               \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::info(                                                   \
      LOG_TAG.data(), LOG_FORMAT(I, format), ##__VA_ARGS__);                   \
  } while (false)

#define PN_LOG_WARN(format, ...)                                               \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::warn(                                                   \
      LOG_TAG.data(), LOG_FORMAT(W, format), ##__VA_ARGS__);                    \
  } while(false)

#define PN_LOG_ERROR(format, ...)                                              \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::error(                                                  \
      LOG_TAG.data(), LOG_FORMAT(E, format), ##__VA_ARGS__);                   \
  } while (false)

#define PN_LOG_DEBUG(format, ...)                                              \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::debug(                                                  \
      LOG_TAG.data(), LOG_FORMAT(D, format), ##__VA_ARGS__);                   \
  } while (false)

#define PN_LOG_VERBOSE(format, ...)                                            \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::verbose(                                                \
      LOG_TAG.data(), LOG_FORMAT(V, format), ##__VA_ARGS__);                   \
  } while (false)

#define PN_LOG_INVALID_DATA(format, ...)                                       \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::invalidData(LOG_TAG.data(), format, ##__VA_ARGS__);     \
  } while (false)

#define PN_LOG_UNREACHABLE()                                                   \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    pn::internal::log::location(LOG_TAG.data());                               \
    pn::internal::log::error(                                                  \
      LOG_TAG.data(), LOG_FORMAT(E, "Unreachable code"));                      \
  } while (false)

#endif // PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H
