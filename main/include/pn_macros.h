#ifndef PROJECT_PN_MIKROSKOP_ESP32_PN_MACROS_H
#define PROJECT_PN_MIKROSKOP_ESP32_PN_MACROS_H

#include <esp_bit_defs.h>
#include <esp_check.h>
#include "../pn_config.h"

namespace Status {
inline constexpr esp_err_t SUCCESS = ESP_OK;
inline constexpr esp_err_t FAIL    = BIT0;
} // namespace Status

#if DEBUG == 1

#include "pn_logger.h"

#define PN_VALIDATE_TAG()                                                      \
  static_assert(                                                               \
    std::is_same_v<std::remove_cv_t<decltype(LOG_TAG)>, std::string_view>,     \
    "LOG_TAG must be a string_view")

#define PN_ERR_CHECK(err)                                                      \
  PN_VALIDATE_TAG();                                                           \
  do {                                                                         \
    if ((err) != Status::SUCCESS) {                                            \
      pn::log::error(LOG_TAG.data());                                          \
      return Status::FAIL;                                                     \
    }                                                                          \
  } while (false)

#define PN_FAIL()                                                              \
  PN_VALIDATE_TAG();                                                           \
  do {                                                                         \
    pn::log::error(LOG_TAG.data(), "Fail");                                    \
    return Status::FAIL;                                                       \
  } while (false)

#define PN_UNIMPLEMENTED()                                                     \
  PN_VALIDATE_TAG();                                                           \
  do {                                                                         \
    pn::log::error(LOG_TAG.data(), "Unimplemented");                           \
    return Status::FAIL;                                                       \
  } while (false)

#else // DEBUG == 0

#define PN_ERR_CHECK(err)                                                      \
  do {                                                                         \
    if ((err) != Status::SUCCESS) { return Status::FAIL; }                     \
  } while (false)

#define PN_FAIL()                                                              \
  do { return Status::FAIL } while (false)

#define PN_UNIMPLEMENTED() PN_FAIL()

#endif // DEBUG

#endif // PROJECT_PN_MIKROSKOP_ESP32_PN_MACROS_H