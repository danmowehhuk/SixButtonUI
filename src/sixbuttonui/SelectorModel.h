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

    // Initialize the option arrays with this many elements. This must be called before setting options.
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
    void setOptionRaw(uint8_t i, const char* name, bool isNamePmem, const char* value, bool isValuePmem, 
          bool allocateName = true, bool allocateValue = true);

    // (Optional) 
    // Store the previously selected value. If this value is selected, isCurrValueSelected() will return
    // true. This can be used by the user's render function to indicate that selecting this value will
    // be a no-op. Note: Setting the current value does not cause the option to be preselected. Use the
    // selectOptionWith...() methods to preselect the option.
    void setCurrValue(const char* currValue, bool allocate = true);
    void setCurrValue(const __FlashStringHelper* currValue);
    void setCurrValueRaw(const char* currValue, bool isPmem, bool allocate);
    bool isCurrValueSelected();

    // (Optional)
    // When used with a ComboBoxWidget, this value will be prefilled in the text field. Note: Setting 
    // this value may not cause any option to be preselected, nor does it guarantee that the option will
    // even be selectable if the model function doesn't return a matching result for this search prefix.
    void setInitialSearchPrefix(const char* searchPrefix, bool allocate = true);

    // (Optional)
    // Set an initial selection. This will cause the option to be preselected if it exists.
    bool selectOptionWithName(const char* name);
    bool selectOptionWithName(const __FlashStringHelper* name);
    bool selectOptionWithValue(const char* value);
    bool selectOptionWithValue(const __FlashStringHelper* value);

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
    bool _isCurrValuePmem = false;
    bool _isOwnsCurrValue = false;
    const char* _searchPrefix = nullptr;
    bool _isOwnsSearchPrefix = false;
    const char* _initialSearchPrefix = nullptr;
    bool _isOwnsInitialSearchPrefix = false;
    const char* _selectionName = nullptr;
    bool _isSelectionNamePmem = false;
    bool _isOwnsSelectionName = false;
    bool selectOptionWithNameRaw(const char* name, bool isNamePmem);
    bool selectOptionWithValueRaw(const char* value, bool isValuePmem);
    bool selectOptionBy(const char* key, bool isKeyPmem, const char** arr, const bool* isPmemArr);
    void setSearchPrefix(const char* searchPrefix, bool allocate = true);
    void resetOptions();
    void clear();

    // All of the following are backed by SelectorModels and need access to the private members.
    // The private members should not be accessible by the user's model functions.
    friend class SelectorWidget;
    friend class SubMenuWidget;
    friend class ComboBoxWidget;
    friend class WizardModel;

};


#endif
