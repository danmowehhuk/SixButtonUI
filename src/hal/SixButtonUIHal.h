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

#ifdef SIXBUTTONUI_SERIAL_PORT
#error "SIXBUTTONUI_SERIAL_PORT is Arduino-only; under NO_ARDUINO SixButtonUI always uses BareMetalHAL::Uart0"
#endif

// All I/O, including the SIXBUTTONUI_ENABLE_MCP wire protocol, is on
// UART0. The caller owns its init: Uart0::begin(), plus Uart0::enableRx()
// when MCP is enabled.
inline void print(const FlashStr* s) { BareMetalHAL::Uart0::print(s); }
inline void print(const char* s) { BareMetalHAL::Uart0::print(s); }
inline void print(char c) { BareMetalHAL::Uart0::print(c); }
inline void print(int i) { BareMetalHAL::Uart0::print(i); }
inline void println(const char* s) { BareMetalHAL::Uart0::println(s); }
inline void println(int i) { BareMetalHAL::Uart0::println(i); }
inline void delay(uint16_t ms) { BareMetalHAL::delay(ms); }
inline int available() { return BareMetalHAL::Uart0::available(); }
inline int read() { return BareMetalHAL::Uart0::read(); }

#endif

}  // namespace SixButtonUIHal

#endif
