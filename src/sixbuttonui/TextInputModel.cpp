#include "TextInputModel.h"

TextInputModel::TextInputModel(const char* initValue, bool pmem = false) {
  if (initValue) {
    initialize(initValue, pmem);
  } else {
    _valueLen = 1;
    _value = new char[_valueLen + 1] { '\0' };
    _isInitialized = false;
  }
}

void TextInputModel::initialize(const char* initValue, bool pmem = false) {
  if (_value) free(_value);
  if (!initValue || (pmem ? strlen_P(initValue) : strlen(initValue)) == 0) {
    _valueLen = 1;
    _value = new char[_valueLen + 1] { '\0' };
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

void TextInputModel::initialize(const __FlashStringHelper* initValue) {
  initialize(reinterpret_cast<const char*>(initValue), true);
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
