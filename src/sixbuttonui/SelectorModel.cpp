#include "SelectorModel.h"

void SelectorModel::setNumOptions(uint8_t numOptions) {
  _numOptions = numOptions;
  if (numOptions > 0) {
    _optionNames = new const char*[_numOptions]();
    _isOptionNamePmem = new bool[_numOptions]();
    _isOwnsOptionName = new bool[_numOptions]();
    _optionValues = new const char*[_numOptions]();
    _isOptionValuePmem = new bool[_numOptions]();
    _isOwnsOptionValue = new bool[_numOptions]();
  }
}

void SelectorModel::setOption(uint8_t i, const char* name, const char* value, 
      bool allocateName = false, bool allocateValue = false) {
  setOptionRaw(i, name, false, value, false, allocateName, allocateValue);
}

void SelectorModel::setOption(uint8_t i, const __FlashStringHelper* name, 
      const char* value, bool allocateValue = false) {
  setOptionRaw(i, reinterpret_cast<const char*>(name), true, value, false, false, allocateValue);
}

void SelectorModel::setOption(uint8_t i, const char* name, 
      const __FlashStringHelper* value, bool allocateName = false) {
  setOptionRaw(i, name, false, reinterpret_cast<const char*>(value), true, allocateName, false);
}

void SelectorModel::setOption(uint8_t i, const __FlashStringHelper* name, 
      const __FlashStringHelper* value) {
  setOptionRaw(i, reinterpret_cast<const char*>(name), true, reinterpret_cast<const char*>(value), true, false, false);
}

void SelectorModel::setOptionRaw(uint8_t i, const char* name, bool namePmem, const char* value, bool valuePmem, 
      bool allocateName = false, bool allocateValue = false) {
  if (_optionNames[i] && _isOwnsOptionName[i]) {
    free(_optionNames[i]);
  }
  if (_optionValues[i] && _isOwnsOptionValue[i]) {
    free(_optionValues[i]);
  }
  _optionNames[i] = allocateName ? strdup(name) : name;
  _isOwnsOptionName[i] = allocateName;
  _isOptionNamePmem[i] = namePmem;
  _optionValues[i] = allocateValue ? strdup(value) : value;
  _isOwnsOptionValue[i] = allocateValue;
  _isOptionValuePmem[i] = valuePmem;
}

void SelectorModel::setCurrValue(const char* currValue, bool allocate = false) {
  if (_currValue && _isOwnsCurrValue) {
    free(_currValue);
  }
  _currValue = allocate ? strdup(currValue) : currValue;
  _isOwnsCurrValue = allocate;
}

void SelectorModel::clear() {
  for (uint8_t i = 0; i < _numOptions; i++) {
    if (_optionNames[i] && _isOwnsOptionName[i]) {
      free(_optionNames[i]);
    }
    if (_optionValues[i] && _isOwnsOptionValue[i]) {
      free(_optionValues[i]);
    }
  }
  if (_currValue && _isOwnsCurrValue) {
    free(_currValue);
  }
  delete[] _optionNames;
  delete[] _isOptionNamePmem;
  delete[] _isOwnsOptionName;
  delete[] _optionValues;
  delete[] _isOptionValuePmem;
  delete[] _isOwnsOptionValue;
  _optionNames = nullptr;
  _isOptionNamePmem = nullptr;
  _isOwnsOptionName = nullptr;
  _optionValues = nullptr;
  _isOptionValuePmem = nullptr;
  _isOwnsOptionValue = nullptr;
  _currValue = nullptr;
  _isOwnsCurrValue = false;
  _numOptions = 0;
  _currIndex = 0;
}
