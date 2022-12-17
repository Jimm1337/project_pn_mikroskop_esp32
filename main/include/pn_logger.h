#ifndef PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H
#define PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H

#include "../pn_config.h"
#include "pn_macros.h"

#if DEBUG == 1

#include "esp_log.h"
#include <source_location>

#define PN_LOG_LOCATION()                                                      \
  do {                                                                         \
    /*NOLINT*/ ESP_LOGD(                                                       \
      LOG_TAG.data(),                                                          \
      "<Loc>: %s",                                                             \
      std::source_location::current().function_name());                        \
  } while (false)

#define PN_LOG_INFO(format, ...)                                               \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGI(LOG_TAG.data(), format, ##__VA_ARGS__);                \
  } while (false)

#define PN_LOG_WARN(format, ...)                                               \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGW(LOG_TAG.data(), format, ##__VA_ARGS__);                \
  } while (false)

#define PN_LOG_ERROR(format, ...)                                              \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGE(LOG_TAG.data(), format, ##__VA_ARGS__);                \
  } while (false)

#define PN_LOG_DEBUG(format, ...)                                              \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGD(LOG_TAG.data(), format, ##__VA_ARGS__);                \
  } while (false)

#define PN_LOG_VERBOSE(format, ...)                                            \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGV(LOG_TAG.data(), format, ##__VA_ARGS__);                \
  } while (false)

#define PN_LOG_INVALID_DATA(format, ...)                                       \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGW(LOG_TAG.data(), "INVALID DATA!");                      \
    /*NOLINT*/ ESP_LOGW(LOG_TAG.data(), format, ##__VA_ARGS__);                \
  } while (false)

#define PN_LOG_UNREACHABLE()                                                   \
  do {                                                                         \
    PN_VALIDATE_TAG();                                                         \
    PN_LOG_LOCATION();                                                         \
    /*NOLINT*/ ESP_LOGE(LOG_TAG.data(), "!!!UNREACHABLE!!!");                  \
  } while (false)

#else // DEBUG == 0

#define PN_LOG_LOCATION(format, ...)                                           \
  do {                                                                         \
  } while (false)

#define PN_LOG_INFO(format, ...)                                               \
  do {                                                                         \
  } while (false)

#define PN_LOG_WARN(format, ...)                                               \
  do {                                                                         \
  } while (false)

#define PN_LOG_ERROR(format, ...)                                              \
  do {                                                                         \
  } while (false)

#define PN_LOG_DEBUG(format, ...)                                              \
  do {                                                                         \
  } while (false)

#define PN_LOG_VERBOSE(format, ...)                                            \
  do {                                                                         \
  } while (false)

#define PN_LOG_INVALID_DATA(format, ...)                                       \
  do {                                                                         \
  } while (false)

#define PN_LOG_UNREACHABLE()                                                   \
  do {                                                                         \
  } while (false)

#endif // DEBUG

#endif // PROJECT_PN_MIKROSKOP_ESP32_PN_LOGGER_H
