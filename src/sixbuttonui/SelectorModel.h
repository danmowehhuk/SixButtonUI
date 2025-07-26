#ifndef _sixbuttonui_SelectorModel_h
#define _sixbuttonui_SelectorModel_h


#include "WidgetModel.h"

/*
 * SelectorModel:
 * - title (from WidgetModel)
 * - instruction (from WidgetModel)
 * - options... (name/value pairs)
 * - numOptions (number of options)
 * - currValue (value of the currently selected option)
 * - currIndex (determined based on currValue)
 * - footer (from WidgetModel)
 */
class SelectorModel : public WidgetModel {
  public:
    SelectorModel() {};
    ~SelectorModel() override { clear(); };

    /*
     * Methods for populating the model
     */

    // Initialize the option arrays with this many elements
    void setNumOptions(uint8_t numOptions);

    // Multiple ways of adding a name/value pair to the list of options.
    // By default, char*'s are strdup'ed into their own arrays managed by
    // this class. If using string literals, you can set allocate...=false
    // to prevent freeing of the memory
    //
    // When used with a ComboBoxWidget, the options should be set to 
    // completions that will be appended to the search prefix
    void setOption(uint8_t index, const char* name, const char* value, bool allocateName = true, bool allocateValue = true);
    void setOption(uint8_t index, const __FlashStringHelper* name, const char* value, bool allocateValue = true);
    void setOption(uint8_t index, const char* name, const __FlashStringHelper* value, bool allocateName = true);
    void setOption(uint8_t index, const __FlashStringHelper* name, const __FlashStringHelper* value);
    void setOptionRaw(uint8_t i, const char* name, bool namePmem, const char* value, bool valuePmem, 
          bool allocateName = false, bool allocateValue = false);
    void setCurrValue(const char* currValue, bool allocate = true);

    // Set an initial selection
    bool selectOptionWithName(char* name);
    bool selectOptionWithValue(char* value);

    // Methods for getting values from the model
    uint8_t getCurrIndex()        { return _currIndex; };
    uint8_t getNumOptions()       { return _numOptions; };
    bool isOptionNamePmem()       { return _isOptionNamePmem[_currIndex]; };
    bool isOptionValuePmem()      { return _isOptionValuePmem[_currIndex]; };
    const char* getOptionName()   { return _optionNames[_currIndex]; };
    const char* getOptionValue()  { return _optionValues[_currIndex]; };
    const char* getSearchPrefix() { return _searchPrefix; };

    // Disable moving and copying
    SelectorModel(SelectorModel&& other) = delete;
    SelectorModel& operator=(SelectorModel&& other) = delete;
    SelectorModel(const SelectorModel&) = delete;
    SelectorModel& operator=(const SelectorModel&) = delete;

  protected:
    uint8_t _numOptions = 0;
    uint8_t _currIndex = 0;
    void prev() {
      if (_currIndex > 0) _currIndex--;
    };
    void next() {
      if (_currIndex < _numOptions - 1) _currIndex++;
    };

  private:
    const char** _optionNames = nullptr;
    bool* _isOptionNamePmem = nullptr;
    bool* _isOwnsOptionName = nullptr;
    const char** _optionValues = nullptr;
    bool* _isOptionValuePmem = nullptr;
    bool* _isOwnsOptionValue = nullptr;
    const char* _currValue = nullptr;
    bool _isOwnsCurrValue = false;
    const char* _searchPrefix = nullptr;
    bool _isOwnsSearchPrefix = false;
    bool selectOptionBy(char* key, const char** arr, const bool* isPmemArr);
    void setSearchPrefix(const char* searchPrefix, bool allocate = true);
    void resetOptions();
    void clear();

    friend class SelectorWidget;
    friend class SubMenuWidget;
    friend class ComboBoxWidget;
    friend class WizardModel;

};


#endif
