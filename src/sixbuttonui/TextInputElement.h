#ifndef _sixbuttonui_TextInputElement_h
#define _sixbuttonui_TextInputElement_h


#include "TextInputModel.h"
#include "UIElement.h"

class TextInputElement: public UIElementBase<TextInputElement> {

  public:
    TextInputElement(): UIElementBase(UIElement::Type::TEXT_INPUT) {};
    TextInputElement(uint8_t id): UIElementBase(UIElement::Type::TEXT_INPUT, id) {};

    TextInputElement* withInitialValue(const char* initValue, bool pmem = false) {
      _initValue = initValue;
      _isInitValuePmem = pmem;
      return this;
    };

    TextInputElement* withInitialValue(const __FlashStringHelper* initValue) {
      return withInitialValue(reinterpret_cast<const char*>(initValue), true);
    }

    char* getInitialValue() { return _initValue; };
    bool isInitialValuePmem() { return _isInitValuePmem; };

    typedef void (*ModelFunction)(TextInputModel* model, void* state);
    ModelFunction modelLoader = 0;
    TextInputElement* withModelFunction(ModelFunction modelFunction) {
      modelLoader = modelFunction;
      return this;
    };

    typedef void* (*OnEnterFunction)(char* value, void* state);
    OnEnterFunction onEnterFunc = 0;
    TextInputElement* onEnter(OnEnterFunction func) {
      onEnterFunc = func;
      return this;
    };

  private:
    TextInputElement(TextInputElement &t) = delete;
    char* _initValue = nullptr;
    bool _isInitValuePmem = false;

};


#endif