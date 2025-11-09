#include "SelectorModel.h"

void SelectorModel::setNumOptions(uint8_t numOptions) {
  if (_numOptions > 0) {
    resetOptions();
  }
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
      bool allocateName, bool allocateValue) {
  setOptionRaw(i, name, false, value, false, allocateName, allocateValue);
}

void SelectorModel::setOption(uint8_t i, const __FlashStringHelper* name, 
      const char* value, bool allocateValue) {
  setOptionRaw(i, reinterpret_cast<const char*>(name), true, value, false, false, allocateValue);
}

void SelectorModel::setOption(uint8_t i, const char* name, 
      const __FlashStringHelper* value, bool allocateName) {
  setOptionRaw(i, name, false, reinterpret_cast<const char*>(value), true, allocateName, false);
}

void SelectorModel::setOption(uint8_t i, const __FlashStringHelper* name, 
      const __FlashStringHelper* value) {
  setOptionRaw(i, reinterpret_cast<const char*>(name), true, reinterpret_cast<const char*>(value), true, false, false);
}

void SelectorModel::setOptionRaw(uint8_t i, const char* name, bool namePmem, const char* value, bool valuePmem, 
      bool allocateName, bool allocateValue) {
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

void SelectorModel::setCurrValue(const char* currValue, bool allocate) {
  if (_currValue && _isOwnsCurrValue) {
    free(_currValue);
  }
  _currValue = allocate ? strdup(currValue) : currValue;
  _isOwnsCurrValue = allocate;
}

bool SelectorModel::isCurrValueSelected() {
  if (_currValue && _isOptionValuePmem[_currIndex] 
      && strcmp_P(_currValue, _optionValues[_currIndex]) == 0) {
    return true;
  } else if (_currValue && !_isOptionValuePmem[_currIndex] 
      && strcmp(_currValue, _optionValues[_currIndex]) == 0) {
    return true;
  } else {
    return false;
  }
}

void SelectorModel::setSearchPrefix(const char* searchPrefix, bool allocate) {
  if (_searchPrefix && _isOwnsSearchPrefix) {
    free(_searchPrefix);
  }
  _searchPrefix = allocate ? strdup(searchPrefix) : searchPrefix;
  _isOwnsSearchPrefix = allocate;
}

bool SelectorModel::selectOptionWithValue(char* value) {
  return selectOptionBy(value, _optionValues, _isOptionValuePmem);
}

bool SelectorModel::selectOptionWithName(char* name) {
  return selectOptionBy(name, _optionNames, _isOptionNamePmem);
}

bool SelectorModel::selectOptionBy(char* key, const char** arr, const bool* isPmemArr) {
  _currIndex = 0;
  if (!key || strlen(key) == 0) return true;
  bool match = false;
  for (uint8_t i = 0; i < _numOptions; i++) {
    if ((!isPmemArr[i] && strcmp(key, arr[i]) == 0)
        || (isPmemArr[i] && strcmp_P(key, arr[i]) == 0)) {
      _currIndex = i;
      match = true;
      break;
    }
  }
#if defined(DEBUG)
  if (!match) {
    Serial.print(F("Option not found for key: "));
    Serial.println(key);
  }
#endif
  return match;
}

void SelectorModel::resetOptions() {
  for (uint8_t i = 0; i < _numOptions; i++) {
    setOptionRaw(i, nullptr, false, nullptr, false, false, false);
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
  _numOptions = 0;
}

void SelectorModel::clear() {
  resetOptions();
  if (_currValue && _isOwnsCurrValue) {
    free(_currValue);
  }
  if (_searchPrefix && _isOwnsSearchPrefix) {
    free(_searchPrefix);
  }
  _currValue = nullptr;
  _isOwnsCurrValue = false;
  _searchPrefix = nullptr;
  _isOwnsSearchPrefix = false;
  _currIndex = 0;
}
