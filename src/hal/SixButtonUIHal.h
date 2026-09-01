#ifndef SIXBUTTONUI_HAL_SIXBUTTONUIHAL_H
#define SIXBUTTONUI_HAL_SIXBUTTONUIHAL_H

#include <stdint.h>
#include "FlashStr.h"

#ifndef NO_ARDUINO
#include <Arduino.h>
#endif

namespace SixButtonUIHal {

#ifndef NO_ARDUINO

// The HardwareSerial instance used for all Arduino-mode I/O below,
// including the SIXBUTTONUI_ENABLE_MCP wire protocol. Override with
// e.g. -DSIXBUTTONUI_SERIAL_PORT=Serial2 to move it off UART0.
#ifndef SIXBUTTONUI_SERIAL_PORT
#define SIXBUTTONUI_SERIAL_PORT Serial
#endif

inline void print(const FlashStr* s) { SIXBUTTONUI_SERIAL_PORT.print(s); }
inline void print(const char* s) { SIXBUTTONUI_SERIAL_PORT.print(s); }
inline void print(char c) { SIXBUTTONUI_SERIAL_PORT.print(c); }
inline void print(int i) { SIXBUTTONUI_SERIAL_PORT.print(i); }
inline void println(const char* s) { SIXBUTTONUI_SERIAL_PORT.println(s); }
inline void println(int i) { SIXBUTTONUI_SERIAL_PORT.println(i); }
inline void delay(uint16_t ms) { ::delay(ms); }
inline int available() { return SIXBUTTONUI_SERIAL_PORT.available(); }
inline int read() { return SIXBUTTONUI_SERIAL_PORT.read(); }

#else

#include <BareMetalHAL.h>

inline void print(const FlashStr* s) { BareMetalHAL::Uart0::print(s); }
inline void println(const char* s) { BareMetalHAL::Uart0::println(s); }
inline void println(int i) { BareMetalHAL::Uart0::println(i); }
inline void delay(uint16_t ms) { BareMetalHAL::delay(ms); }

#endif

}  // namespace SixButtonUIHal

#endif
