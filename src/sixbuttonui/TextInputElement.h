#ifndef _sixbuttonui_TextInputElement_h
#define _sixbuttonui_TextInputElement_h


#include "TextInputModel.h"
#include "UIElement.h"
#include "../hal/FlashStr.h"

class TextInputElement: public UIElementBase<TextInputElement> {

  public:
    TextInputElement(): UIElementBase(UIElement::Type::TEXT_INPUT) {};
    TextInputElement(uint8_t id): UIElementBase(UIElement::Type::TEXT_INPUT, id) {};

    TextInputElement* withInitialValue(const char* initValue, bool pmem = false) {
      _initValue = initValue;
      _isInitValuePmem = pmem;
      return this;
    };

    TextInputElement* withInitialValue(const FlashStr* initValue) {
      return withInitialValue(reinterpret_cast<const char*>(initValue), true);
    }

    const char* getInitialValue() { return _initValue; };
    bool isInitialValuePmem() { return _isInitValuePmem; };

    // Limits entry to maxLength characters, e.g. the width of the display
    // that shows it: the cursor can't move past the last one, and starts
    // there when the initial value is longer. A longer initial value isn't
    // cut: onEnter gets it whole. Without it, the limit is
    // TextInputModel::MAX_LENGTH.
    // Params:
    //   maxLength - 1 to TextInputModel::MAX_LENGTH; anything else means
    //               MAX_LENGTH
    // Returns this element.
    TextInputElement* withMaxLength(uint8_t maxLength) {
      bool valid = maxLength > 0 && maxLength <= TextInputModel::MAX_LENGTH;
      _maxLength = valid ? maxLength : TextInputModel::MAX_LENGTH;
      return this;
    };

    uint8_t getMaxLength() { return _maxLength; };

    typedef void (*ModelFunction)(TextInputModel* model, void* state);
    ModelFunction modelLoader = 0;
    TextInputElement* withModelFunction(ModelFunction modelFunction) {
      modelLoader = modelFunction;
      return this;
    };

    typedef void* (*OnEnterFunction)(const char* value, void* state);
    OnEnterFunction onEnterFunc = 0;
    TextInputElement* onEnter(OnEnterFunction func) {
      onEnterFunc = func;
      return this;
    };

  private:
    TextInputElement(TextInputElement &t) = delete;
    const char* _initValue = nullptr;
    bool _isInitValuePmem = false;
    uint8_t _maxLength = TextInputModel::MAX_LENGTH;

};


#endif