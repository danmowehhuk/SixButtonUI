#ifndef _sixbuttonui_mcp_Mcp_h
#define _sixbuttonui_mcp_Mcp_h

#if defined(SIXBUTTONUI_ENABLE_MCP)

// This protocol uses UART0: Serial in Arduino mode, BareMetalHAL::Uart0
// under NO_ARDUINO, where the caller must call
// BareMetalHAL::Uart0::enableRx() after Uart0::begin().

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

  // The display line mirrors what the render function draws, as one
  // "6BUI=>'@<row>,<col>;<text>@<row>,<col>;<text>...'" line sent after
  // the view model's. Each <text> is the characters drawn from that
  // position on, with '@', '\'' and '\\' backslash-escaped, and any
  // character outside printable ASCII (control characters, an LCD's
  // custom or ROM characters) sent as "\x" plus two hex digits, so it
  // can't break the line. Drawing before the first position starts at
  // row 0, column 0.
  //
  // beginDisplay() and endDisplay() bracket one render. The line opens
  // at the first displayCursor() or displayChar() between them, so a
  // render function that draws nothing sends nothing, and calls outside
  // them are ignored.
  void beginDisplay();
  void displayCursor(uint8_t col, uint8_t row);
  void displayChar(char c);
  void endDisplay();

  // The longest a character's display line text gets, "\xHH", plus the
  // terminator.
  const uint8_t DISPLAY_TEXT_SIZE = 5;

  // Converts one drawn character to its display line text.
  // Params:
  //   c    - the character
  //   text - gets the text, null-terminated
  void toDisplayText(char c, char text[DISPLAY_TEXT_SIZE]);

} // namespace Mcp

#endif // SIXBUTTONUI_ENABLE_MCP
#endif
