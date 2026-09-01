#ifndef _sixbuttonui_mcp_Mcp_h
#define _sixbuttonui_mcp_Mcp_h

#if defined(SIXBUTTONUI_ENABLE_MCP)

#if defined(NO_ARDUINO)
#error "SIXBUTTONUI_ENABLE_MCP requires BareMetalHAL UART RX, not yet implemented"
#endif

// In Arduino mode, this protocol reads/writes whichever HardwareSerial
// instance SixButtonUIHal.h resolves SIXBUTTONUI_SERIAL_PORT to
// (Serial/UART0 by default; override with e.g.
// -DSIXBUTTONUI_SERIAL_PORT=Serial2 to use a different UART).

#include <stdint.h>
#include "../sixbuttonui/ViewModel.h"

namespace Mcp {

  enum class Code : uint8_t {
    NONE,
    UP, DN, LF, RT, EN, ME,
    UP_L, DN_L, LF_L, RT_L, EN_L, ME_L
  };

  // True for the 6 "-L" (long-press) codes.
  bool isLongVariant(Code code);

  // Incrementally parses "6BUI: <CODE>\n" lines fed one byte at a
  // time (e.g. from Serial.read()). Returns Code::NONE until a
  // complete, valid line has been recognized. Bytes not matching the
  // "6BUI: " prefix, or a line too long to fit the internal buffer,
  // are discarded silently; the parser resyncs at the next '\n'.
  class LineParser {
    public:
      Code feed(char c);

    private:
      static const uint8_t BUFFER_SIZE = 24;
      char _buf[BUFFER_SIZE];
      uint8_t _len = 0;
      bool _overflowed = false;

      void reset();
      Code parseLine();
  };

  // Copies src into dest, backslash-escaping ';', '=', '\'', and '\\'.
  // Truncates (never overflows dest) if the escaped result would not
  // fit; dest is always null-terminated. src may be a PROGMEM pointer
  // (pmem = true) or a plain RAM pointer (pmem = false); nullptr is
  // treated as an empty string.
  void escapeInto(char* dest, uint8_t destSize, const char* src, bool pmem);

  // Writes one "6BUI->'...'" line via SixButtonUIHal. Field order:
  // type;id;title;instr;interactive;footer;cursor;cursorPos;hasNext;
  // hasPrev;isSelected;isSelectable;isCancelable
  void serialize(ViewModel& vm);

} // namespace Mcp

#endif // SIXBUTTONUI_ENABLE_MCP
#endif
