#ifndef _SixButtonUI_Strings_h
#define _SixButtonUI_Strings_h


#include <Arduino.h>

namespace SixButtonUIStrings {

  char* strdup_P(const char* progmemStr);
  char* strdup(const __FlashStringHelper* progmemStr);

}


#endif
