#include "TextInputModel.h"

TextInputModel::TextInputModel(const char* initValue, bool pmem = false) {
  setInitialValue(initValue, pmem);
}

void TextInputModel::setInitialValue(const char* initValue, bool pmem = false) {
  if (_value) free(_value);
  if (!initValue || (pmem ? strlen_P(initValue) : strlen(initValue)) == 0) {
    setInitialValue(" ", false);
  } else {
    _valueLen = pmem ? strlen_P(initValue) : strlen(initValue);
    _value = new char[_valueLen + 1];
    if (pmem) {
      strncpy_P(_value, initValue, _valueLen);
    } else {
      strncpy(_value, initValue, _valueLen);
    }
    _value[_valueLen] = '\0';
  }
  _cursorPos = _valueLen - 1;
  _isInitialized = true;
}

void TextInputModel::setInitialValue(const __FlashStringHelper* initValue) {
  setInitialValue(reinterpret_cast<const char*>(initValue), true);
}

void TextInputModel::setCursorPos(uint8_t pos) {
  if (pos >= _valueLen) {
    if (pos < MAX_LENGTH) {
      char* oldValue = _value;
      _value = new char[_valueLen + 2];
      strncpy(_value, oldValue, _valueLen);
      delete[] oldValue;
      _value[_valueLen] = ' ';
      pos = _valueLen;
      _valueLen++;
      _value[_valueLen] = '\0';
    } else {
      pos = MAX_LENGTH - 1;
    }
  }
  _cursorPos = pos;
}

void TextInputModel::setChar(char c) {
  _value[_cursorPos] = c;
}

char TextInputModel::getChar() {
  return _value[_cursorPos];
}

char* TextInputModel::getValue() { 
  return _value;
}

void TextInputModel::clear() {
  if (_value) free(_value);
  _value = nullptr;
}

char TextInputModel::getNext(char c) {
  uint8_t curr = (uint8_t)c;
  uint8_t next = curr;
  switch (curr) {  // always start with ' ', then
    case 32:       // lower-case, upper-case, numbers, then symbols
      next = 97;
      break;
    case 122:
      next = 65;
      break;
    case 90:
      next = 48;
      break;
    case 57:
      next = 33;
      break;
    case 47:
      next = 58;
      break;
    case 64:
      next = 91;
      break;
    case 96:
      next = 123;
      break;
    case 126:
      next = 32;
      break;
    default:
      next++;
      break;
  }
  return (char)next;
}

char TextInputModel::getPrev(char c) {
  uint8_t curr = (uint8_t)c;
  uint8_t prev = curr;
  switch (curr) {  // same as onDownPressed, but reverse order
    case 32:
      prev = 126;
      break;
    case 123:
      prev = 96;
      break;
    case 91:
      prev = 64;
      break;
    case 58:
      prev = 47;
      break;
    case 33:
      prev = 57;
      break;
    case 48:
      prev = 90;
      break;
    case 65:
      prev = 122;
      break;
    case 97:
      prev = 32;
      break;
    default:
      prev--;
      break;
  }
  return (char)prev;
}
