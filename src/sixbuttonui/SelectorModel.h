#ifndef _sixbuttonui_SelectorModel_h
#define _sixbuttonui_SelectorModel_h


#include "WidgetModel.h"

class SelectorModel : public WidgetModel {
  public:
    SelectorModel() {};
    ~SelectorModel() override { clear(); };
    void setNumOptions(uint8_t numOptions);
    uint8_t getCurrIndex() { return _currIndex; };
    uint8_t getNumOptions() { return _numOptions; };
    void setOption(uint8_t index, const char* name, const char* value, bool allocateName = false, bool allocateValue = false);
    void setOption(uint8_t index, const __FlashStringHelper* name, const char* value, bool allocateValue = false);
    void setOption(uint8_t index, const char* name, const __FlashStringHelper* value, bool allocateName = false);
    void setOption(uint8_t index, const __FlashStringHelper* name, const __FlashStringHelper* value);
    void setOptionRaw(uint8_t i, const char* name, bool namePmem, const char* value, bool valuePmem, 
          bool allocateName = false, bool allocateValue = false);
    void setCurrValue(const char* currValue, bool allocate = false);

    // Selection data for the current index
    bool isOptionNamePmem() { return _isOptionNamePmem[_currIndex]; };
    bool isOptionValuePmem() { return _isOptionValuePmem[_currIndex]; };
    const char* getOptionName() { return _optionNames[_currIndex]; };
    const char* getOptionValue() { return _optionValues[_currIndex]; };

    // Disable moving and copying
    SelectorModel(SelectorModel&& other) = delete;
    SelectorModel& operator=(SelectorModel&& other) = delete;
    SelectorModel(const SelectorModel&) = delete;
    SelectorModel& operator=(const SelectorModel&) = delete;

  private:
    const char** _optionNames = nullptr;
    bool* _isOptionNamePmem = nullptr;
    bool* _isOwnsOptionName = nullptr;
    const char** _optionValues = nullptr;
    bool* _isOptionValuePmem = nullptr;
    bool* _isOwnsOptionValue = nullptr;
    const char* _currValue = nullptr;
    bool _isOwnsCurrValue = false;
    uint8_t _numOptions = 0;
    uint8_t _currIndex = 0;
    void clear();

    friend class SelectorWidget;

};


#endif