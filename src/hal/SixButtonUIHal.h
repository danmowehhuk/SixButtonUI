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

#include <BareMetalHAL.h>

inline void print(const FlashStr* s) { BareMetalHAL::Uart0::print(s); }
inline void println(const char* s) { BareMetalHAL::Uart0::println(s); }
inline void println(int i) { BareMetalHAL::Uart0::println(i); }
inline void delay(uint16_t ms) { BareMetalHAL::delay(ms); }

#endif

}  // namespace SixButtonUIHal

#endif
