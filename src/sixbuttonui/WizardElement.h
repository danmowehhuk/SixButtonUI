#ifndef _sixbuttonui_WizardElement_h
#define _sixbuttonui_WizardElement_h


#include "SelectorElement.h"
#include "UIElement.h"
#include "WizardModel.h"

class WizardElement: public UIElementBase<WizardElement> {

  public:
    WizardElement(): UIElementBase(UIElement::Type::WIZARD) {};

    template <typename... Args>
    WizardElement* withSteps(SelectorElement* selectorElement, Args... moreSelectorElements) {
      return static_cast<WizardElement*>(withChildren(selectorElement, moreSelectorElements...));
    };

    typedef void (*WizardModelFunction)(WizardModel* model, void* state);
    WizardModelFunction modelLoader = 0;
    WizardElement* withModelFunction(WizardModelFunction modelFunction) {
      modelLoader = modelFunction;
      return this;
    };

    typedef void* (*WizardOnEnterFunction)(
          char** selectionValues, 
          uint8_t numSelections, 
          void* state);
    WizardOnEnterFunction onEnterFunc = 0;
    WizardElement* onEnter(WizardOnEnterFunction func) {
      onEnterFunc = func;
      return this;
    };

    // Disable moving and copying
    WizardElement(WizardElement&& other) = delete;
    WizardElement& operator=(WizardElement&& other) = delete;
    WizardElement(const WizardElement&) = delete;
    WizardElement& operator=(const WizardElement&) = delete;

  protected:
    WizardElement(UIElement::Type type): UIElementBase(type) {};

};


#endif
