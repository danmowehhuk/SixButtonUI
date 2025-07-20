#include <Arduino.h>
#include "Strings.h"


namespace SixButtonUIStrings {

  bool startsWith(const char* str, const char* prefix) {
    if (!str || !prefix) return false;
    while (*prefix) {
      if (*str++ != *prefix++) return false;
    }
    return true;
  }

  char* strdup_P(const char* progmemStr) {
    if (!progmemStr) return nullptr;
    size_t len = strlen_P(progmemStr);

    // use malloc so cleanup uses free, not delete, just like strdup
    char* ramStr = (char*)malloc(len + 1);
    if (ramStr) {
      strncpy_P(ramStr, progmemStr, len);
      ramStr[len] = '\0';
    }
    return ramStr;
  }

  char* strdup(const __FlashStringHelper* progmemStr) {
    return strdup_P(reinterpret_cast<const char*>(progmemStr));
  }

}
