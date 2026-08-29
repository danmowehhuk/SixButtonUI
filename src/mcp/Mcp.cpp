#include "Mcp.h"

#if defined(SIXBUTTONUI_ENABLE_MCP)

#include <string.h>

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

} // namespace Mcp

#endif // SIXBUTTONUI_ENABLE_MCP
