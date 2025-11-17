#ifndef _sixbuttonui_ComboBoxElement_h
#define _sixbuttonui_ComboBoxElement_h


#include "SelectorElement.h"
#include <Arduino.h>

/*
 * ComboBoxElement allows the user to enter a character string like a TextInputElement,
 * but displays completions of those character strings that can be navigated like
 * a SelectorElement. It uses the same model type and model function signature as
 * a SelectorElement:
 *
 *     typedef void (*SelectorModelFunction)(SelectorModel* model, void* state);
 *
 * On initial load, the model's "currValue" field may be set by the model loader
 * function, but thereafter it will contain the prefix string entered by the user
 * so far, so it should not be modified by the model loader.
 *
 * If a selector option has no value, the enter button will not function.
 *
 * Note that the with* methods are overridden to return ComboBoxElement*
 * so that the methods can be chained.
 */
class ComboBoxElement: public SelectorElement {

  public:
    ComboBoxElement(): SelectorElement(UIElement::Type::COMBO_BOX) {};
    ComboBoxElement(uint8_t id): SelectorElement(UIElement::Type::COMBO_BOX, id) {};

    ComboBoxElement* withTitle(const char* title, bool pmem = false) {
      SelectorElement::withTitle(title, pmem);
      return this;
    };

    ComboBoxElement* withTitle(const __FlashStringHelper* title) {
      SelectorElement::withTitle(title);
      return this;
    };

    ComboBoxElement* withInstruction(const char* instruction, bool pmem = false) {
      SelectorElement::withInstruction(instruction, pmem);
      return this;
    };

    ComboBoxElement* withInstruction(const __FlashStringHelper* instruction) {
      SelectorElement::withInstruction(instruction);
      return this;
    };

    ComboBoxElement* withFooter(const char* footer, bool pmem = false) {
      SelectorElement::withFooter(footer, pmem);
      return this;
    };

    ComboBoxElement* withFooter(const __FlashStringHelper* footer) {
      SelectorElement::withFooter(footer);
      return this;
    };

    ComboBoxElement* withModelFunction(SelectorModelFunction modelFunction) {
      SelectorElement::withModelFunction(modelFunction);
      return this;
    };

    ComboBoxElement* onEnter(SelectorOnEnterFunction func) {
      SelectorElement::onEnter(func);
      return this;
    };

};


#endif
