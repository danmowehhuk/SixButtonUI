#include "Mcp.h"

#if defined(SIXBUTTONUI_ENABLE_MCP)

#include <string.h>
#include <avr/pgmspace.h>
#include "../hal/SixButtonUIHal.h"
#include "../sixbuttonui/UIElement.h"

namespace Mcp {

bool isLongVariant(Code code) {
  switch (code) {
    case Code::UP_L:
    case Code::DN_L:
    case Code::LF_L:
    case Code::RT_L:
    case Code::EN_L:
    case Code::ME_L:
      return true;
    default:
      return false;
  }
}

Code LineParser::feed(char c) {
  if (c == '\n') {
    Code result = parseLine();
    reset();
    return result;
  }
  if (c == '\r') {
    return Code::NONE; // tolerate CRLF line endings
  }
  if (_len < BUFFER_SIZE - 1) {
    _buf[_len++] = c;
  } else {
    _overflowed = true; // line too long; drop until the next '\n'
  }
  return Code::NONE;
}

void LineParser::reset() {
  _len = 0;
  _overflowed = false;
}

Code LineParser::parseLine() {
  if (_overflowed) return Code::NONE;
  _buf[_len] = '\0';

  static const char PREFIX[] = "6BUI: ";
  static const uint8_t PREFIX_LEN = 6;
  if (_len <= PREFIX_LEN || strncmp(_buf, PREFIX, PREFIX_LEN) != 0) {
    return Code::NONE;
  }

  const char* code = _buf + PREFIX_LEN;
  if (strcmp(code, "UP") == 0)   return Code::UP;
  if (strcmp(code, "DN") == 0)   return Code::DN;
  if (strcmp(code, "LF") == 0)   return Code::LF;
  if (strcmp(code, "RT") == 0)   return Code::RT;
  if (strcmp(code, "EN") == 0)   return Code::EN;
  if (strcmp(code, "ME") == 0)   return Code::ME;
  if (strcmp(code, "UP-L") == 0) return Code::UP_L;
  if (strcmp(code, "DN-L") == 0) return Code::DN_L;
  if (strcmp(code, "LF-L") == 0) return Code::LF_L;
  if (strcmp(code, "RT-L") == 0) return Code::RT_L;
  if (strcmp(code, "EN-L") == 0) return Code::EN_L;
  if (strcmp(code, "ME-L") == 0) return Code::ME_L;
  return Code::NONE;
}

void escapeInto(char* dest, uint8_t destSize, const char* src, bool pmem) {
  if (destSize == 0) return;
  uint8_t out = 0;
  if (src) {
    for (uint8_t i = 0; ; i++) {
      char c = pmem ? (char)pgm_read_byte(src + i) : src[i];
      if (c == '\0') break;
      bool special = (c == ';' || c == '=' || c == '\'' || c == '\\');
      uint8_t needed = special ? 2 : 1;
      if (out + needed > destSize - 1) break; // no room left; stop cleanly
      if (special) dest[out++] = '\\';
      dest[out++] = c;
    }
  }
  dest[out] = '\0';
}

namespace {

  const char* typeName(UIElement::Type t) {
    switch (t) {
      case UIElement::Type::ROOT:       return "ROOT";
      case UIElement::Type::SELECTOR:   return "SELECTOR";
      case UIElement::Type::SUB_MENU:   return "SUB_MENU";
      case UIElement::Type::TEXT_INPUT: return "TEXT_INPUT";
      case UIElement::Type::COMBO_BOX:  return "COMBO_BOX";
      case UIElement::Type::WIZARD:     return "WIZARD";
      case UIElement::Type::POPUP:      return "POPUP";
      default:                          return "UNDEFINED";
    }
  }

  const char* cursorModeName(ViewModel::CursorMode m) {
    switch (m) {
      case ViewModel::CursorMode::UNDERLINE: return "UNDERLINE";
      case ViewModel::CursorMode::SOLID:     return "SOLID";
      default:                                return "NO_CURSOR";
    }
  }

  const uint8_t FIELD_BUF_SIZE = 40;

} // namespace

void serialize(ViewModel& vm) {
  char field[FIELD_BUF_SIZE];

  SixButtonUIHal::print("6BUI->'type=");
  SixButtonUIHal::print(typeName(vm.getType()));

  SixButtonUIHal::print(";id=");
  if (vm.hasUIElementId()) {
    SixButtonUIHal::print((int)vm.getUIElementId());
  } else {
    SixButtonUIHal::print("none");
  }

  escapeInto(field, FIELD_BUF_SIZE, vm.getTitleLine(), vm.isTitlePmem());
  SixButtonUIHal::print(";title=");
  SixButtonUIHal::print(field);

  escapeInto(field, FIELD_BUF_SIZE, vm.getInstructionLine(), vm.isInstructionPmem());
  SixButtonUIHal::print(";instr=");
  SixButtonUIHal::print(field);

  escapeInto(field, FIELD_BUF_SIZE, vm.getInteractiveLine(), vm.isInteractivePmem());
  SixButtonUIHal::print(";interactive=");
  SixButtonUIHal::print(field);

  escapeInto(field, FIELD_BUF_SIZE, vm.getFooterLine(), vm.isFooterPmem());
  SixButtonUIHal::print(";footer=");
  SixButtonUIHal::print(field);

  SixButtonUIHal::print(";cursor=");
  SixButtonUIHal::print(cursorModeName(vm.cursorMode));

  SixButtonUIHal::print(";cursorPos=");
  SixButtonUIHal::print((int)vm.cursorPosition);

  SixButtonUIHal::print(";hasNext=");
  SixButtonUIHal::print(vm.hasNext ? "1" : "0");
  SixButtonUIHal::print(";hasPrev=");
  SixButtonUIHal::print(vm.hasPrev ? "1" : "0");
  SixButtonUIHal::print(";isSelected=");
  SixButtonUIHal::print(vm.isSelected ? "1" : "0");
  SixButtonUIHal::print(";isSelectable=");
  SixButtonUIHal::print(vm.isSelectable ? "1" : "0");
  SixButtonUIHal::print(";isCancelable=");
  SixButtonUIHal::print(vm.isCancelable ? "1" : "0");

  SixButtonUIHal::println("'");
}

namespace {

  enum class DisplayState : uint8_t { CLOSED, BEGUN, OPEN };
  DisplayState displayState = DisplayState::CLOSED;

  // Returns true if the display line is open, opening it if a render
  // has begun.
  bool openDisplay() {
    if (displayState == DisplayState::BEGUN) {
      SixButtonUIHal::print(F("6BUI=>'"));
      displayState = DisplayState::OPEN;
    }
    return displayState == DisplayState::OPEN;
  }

  char hexDigit(uint8_t value) {
    return value < 10 ? '0' + value : 'A' + value - 10;
  }

} // namespace

void beginDisplay() {
  displayState = DisplayState::BEGUN;
}

void setDisplayCursor(uint8_t col, uint8_t row) {
  if (openDisplay()) {
    SixButtonUIHal::print('@');
    SixButtonUIHal::print((int)row);
    SixButtonUIHal::print(',');
    SixButtonUIHal::print((int)col);
    SixButtonUIHal::print(';');
  }
}

void toDisplayText(char c, char text[DISPLAY_TEXT_SIZE]) {
  uint8_t code = (uint8_t)c;
  uint8_t length = 0;
  if (code < ' ' || code > '~') {
    text[length++] = '\\';
    text[length++] = 'x';
    text[length++] = hexDigit(code >> 4);
    text[length++] = hexDigit(code & 0x0F);
  } else {
    if (c == '@' || c == '\'' || c == '\\') {
      text[length++] = '\\';
    }
    text[length++] = c;
  }
  text[length] = '\0';
}

void printDisplayChar(char c) {
  if (openDisplay()) {
    char text[DISPLAY_TEXT_SIZE];
    toDisplayText(c, text);
    SixButtonUIHal::print(text);
  }
}

void endDisplay() {
  if (displayState == DisplayState::OPEN) {
    SixButtonUIHal::println("'");
  }
  displayState = DisplayState::CLOSED;
}

} // namespace Mcp

#endif // SIXBUTTONUI_ENABLE_MCP
