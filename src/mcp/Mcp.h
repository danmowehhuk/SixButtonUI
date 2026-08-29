#ifndef _sixbuttonui_mcp_Mcp_h
#define _sixbuttonui_mcp_Mcp_h

#if defined(SIXBUTTONUI_ENABLE_MCP)

#if defined(NO_ARDUINO)
#error "SIXBUTTONUI_ENABLE_MCP requires BareMetalHAL UART RX, not yet implemented"
#endif

#include <stdint.h>

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

} // namespace Mcp

#endif // SIXBUTTONUI_ENABLE_MCP
#endif
