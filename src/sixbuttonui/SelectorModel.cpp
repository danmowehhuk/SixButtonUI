#include "SelectorModel.h"
#include "Strings.h"

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

void SelectorModel::setOptionRaw(uint8_t i, const char* name, bool isNamePmem, const char* value, bool isValuePmem, 
      bool allocateName, bool allocateValue) {
  if (_optionNames[i] && _isOwnsOptionName[i]) {
    free(_optionNames[i]);
  }
  if (_optionValues[i] && _isOwnsOptionValue[i]) {
    free(_optionValues[i]);
  }
  if (!name || isNamePmem || !allocateName) {
    _optionNames[i] = name;
    _isOwnsOptionName[i] = false;
  } else {
    _optionNames[i] = strdup(name);
    _isOwnsOptionName[i] = true;
  }
  _isOptionNamePmem[i] = isNamePmem;
  if (!value || isValuePmem || !allocateValue) {
    _optionValues[i] = value;
    _isOwnsOptionValue[i] = false;
  } else {
    _optionValues[i] = strdup(value);
    _isOwnsOptionValue[i] = true;
  }
  _isOptionValuePmem[i] = isValuePmem;
}

void SelectorModel::setCurrValue(const char* currValue, bool allocate) {
  setCurrValueRaw(currValue, false, allocate);
}

void SelectorModel::setCurrValue(const __FlashStringHelper* currValue) {
  setCurrValueRaw(reinterpret_cast<const char*>(currValue), true, false);
}

void SelectorModel::setCurrValueRaw(const char* currValue, bool isPmem, bool allocate) {
  if (_currValue && _isOwnsCurrValue) {
    free(_currValue);
  }
  if (!currValue || isPmem || !allocate) {
    _currValue = currValue;
    _isOwnsCurrValue = false;
  } else {
    _currValue = strdup(currValue);
    _isOwnsCurrValue = true;
  }
  _isCurrValuePmem = isPmem;
}

void SelectorModel::setInitialSearchPrefix(const char* searchPrefix, bool allocate) {
  if (_initialSearchPrefix && _isOwnsInitialSearchPrefix) {
    free(_initialSearchPrefix);
  }
  if (!searchPrefix || !allocate) {
    _initialSearchPrefix = searchPrefix;
    _isOwnsInitialSearchPrefix = false;
  } else {
    _initialSearchPrefix = strdup(searchPrefix);
    _isOwnsInitialSearchPrefix = true;
  }
}

bool SelectorModel::isCurrValueSelected() {
  // Quick return when comparing two PROGMEM strings (pointer comparison only)
  if (_currValue && _isCurrValuePmem && _isOptionValuePmem[_currIndex]
           && _currValue == _optionValues[_currIndex]) {
    return true;
  }

  bool isMatch = false;
  char* currValueRAM = _currValue;
  if (_currValue && _isCurrValuePmem) currValueRAM = SixButtonUIStrings::strdup_P(_currValue);
  if (_currValue && _isOptionValuePmem[_currIndex] 
      && strcmp_P(_currValue, _optionValues[_currIndex]) == 0) {
    isMatch = true;
  } else if (_currValue && !_isOptionValuePmem[_currIndex] 
      && strcmp(_currValue, _optionValues[_currIndex]) == 0) {
    isMatch = true;
  }
  if (currValueRAM && _isCurrValuePmem) {
    free(currValueRAM);
  }
  return isMatch;
}

void SelectorModel::setSearchPrefix(const char* searchPrefix, bool allocate) {
  if (_searchPrefix && _isOwnsSearchPrefix) {
    free(_searchPrefix);
  }
  if (!searchPrefix || !allocate) {
    _searchPrefix = searchPrefix;
    _isOwnsSearchPrefix = false;
  } else {
    _searchPrefix = strdup(searchPrefix);
    _isOwnsSearchPrefix = true;
  }
}

bool SelectorModel::selectOptionWithValue(const char* value) {
  return selectOptionWithValueRaw(value, false);
}

bool SelectorModel::selectOptionWithValue(const __FlashStringHelper* value) {
  return selectOptionWithValueRaw(reinterpret_cast<const char*>(value), true);
}

bool SelectorModel::selectOptionWithValueRaw(const char* value, bool isValuePmem) {
  return selectOptionBy(value, isValuePmem, _optionValues, _isOptionValuePmem);
}

bool SelectorModel::selectOptionWithName(const char* name) {
  return selectOptionWithNameRaw(name, false);
}

bool SelectorModel::selectOptionWithName(const __FlashStringHelper* name) {
  return selectOptionWithNameRaw(reinterpret_cast<const char*>(name), true);
}

bool SelectorModel::selectOptionWithNameRaw(const char* name, bool isNamePmem) {
  return selectOptionBy(name, isNamePmem, _optionNames, _isOptionNamePmem);
}

bool SelectorModel::selectOptionBy(const char* key, bool isKeyPmem, const char** arr, const bool* isPmemArr) {
  _currIndex = 0;
  if (!key) return true;
  
  // Convert PROGMEM key to RAM if needed, so we can always treat it as RAM
  char* keyRAM = nullptr;
  const char* keyToCompare = key;
  if (isKeyPmem) {
    // Check if key is empty before allocating
    if (strlen_P(key) == 0) return true;
    keyRAM = SixButtonUIStrings::strdup_P(key);
    keyToCompare = keyRAM;
  } else {
    // Check if key is empty
    if (strlen(key) == 0) return true;
  }
  
  bool match = false;
  for (uint8_t i = 0; i < _numOptions; i++) {
    // Try pointer equality first (works for same PROGMEM or same RAM pointer)
    // Then try string comparison - key is always RAM now, array element may be PROGMEM or RAM
    if (key == arr[i] 
        || (isPmemArr[i] ? strcmp_P(keyToCompare, arr[i]) : strcmp(keyToCompare, arr[i])) == 0) {
      _currIndex = i;
      match = true;
      break;
    }
  }
  
  // Free the RAM copy if we allocated it
  if (keyRAM) {
    free(keyRAM);
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
  if (_initialSearchPrefix && _isOwnsInitialSearchPrefix) {
    free(_initialSearchPrefix);
  }
  if (_searchPrefix && _isOwnsSearchPrefix) {
    free(_searchPrefix);
  }
  if (_selectionName && _isOwnsSelectionName) {
    free(_selectionName);
  }
  _currValue = nullptr;
  _isCurrValuePmem = false;
  _isOwnsCurrValue = false;
  _initialSearchPrefix = nullptr;
  _isOwnsInitialSearchPrefix = false;
  _searchPrefix = nullptr;
  _isOwnsSearchPrefix = false;
  _selectionName = nullptr;
  _isSelectionNamePmem = false;
  _isOwnsSelectionName = false;
  _currIndex = 0;
}
