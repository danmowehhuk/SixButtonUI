#ifndef _sixbuttonui_WizardElement_h
#define _sixbuttonui_WizardElement_h


#include "UIElement.h"

class WizardElement: public UIElementBase<WizardElement> {

  public:
  WizardElement(): UIElementBase(UIElement::Type::WIZARD) {};

    // typedef void (*SelectorModelFunction)(SelectorModel* model, void* state);
    // SelectorModelFunction modelLoader = 0;
    // SelectorElement* withModelFunction(SelectorModelFunction modelFunction) {
    //   modelLoader = modelFunction;
    //   return this;
    // };

    // typedef void* (*SelectorOnEnterFunction)(char* selectionName, bool namePmem, char* selectionValue, bool valuePmem, void* state);
    // SelectorOnEnterFunction onEnterFunc = 0;
    // SelectorElement* onEnter(SelectorOnEnterFunction func) {
    //   onEnterFunc = func;
    //   return this;
    // };

    // Disable moving and copying
    WizardElement(WizardElement&& other) = delete;
    WizardElement& operator=(WizardElement&& other) = delete;
    WizardElement(const WizardElement&) = delete;
    WizardElement& operator=(const WizardElement&) = delete;

  protected:
    WizardElement(UIElement::Type type): UIElementBase(type) {};

};


#endif
