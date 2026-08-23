#ifndef SIXBUTTONUI_HAL_SIXBUTTONUIHAL_H
#define SIXBUTTONUI_HAL_SIXBUTTONUIHAL_H

#include <stdint.h>
#include "FlashStr.h"

#ifndef NO_ARDUINO
#include <Arduino.h>
#endif

namespace SixButtonUIHal {

#ifndef NO_ARDUINO

inline void print(const FlashStr* s) { Serial.print(s); }
inline void println(const char* s) { Serial.println(s); }
inline void println(int i) { Serial.println(i); }
inline void delay(uint16_t ms) { ::delay(ms); }

#else

void print(const FlashStr* s);
void println(const char* s);
void println(int i);
void delay(uint16_t ms);

#endif

}  // namespace SixButtonUIHal

#endif
