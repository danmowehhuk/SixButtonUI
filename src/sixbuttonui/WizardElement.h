#ifndef _sixbuttonui_WizardElement_h
#define _sixbuttonui_WizardElement_h


#include "SelectorElement.h"
#include "UIElement.h"
#include "WizardModel.h"

class WizardElement: public UIElementBase<WizardElement> {

  public:
    WizardElement(): UIElementBase(UIElement::Type::WIZARD) {};
    WizardElement(uint8_t id): UIElementBase(UIElement::Type::WIZARD, id) {};

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

    /*
     * ADVANCED CUSTOMIZATION
     *
     * For dynamic wizards (where the number of steps is not known up front), the user can
     * override the default previous/next button behavior.
     *
     * IMPORTANT: Overriding the previous/next button behavior completely bypasses the default
     * behavior, so the user must keep track of the current wizard step and capture the 
     * selection value somewhere.
     */

    // Override callback for previous button. Return true if the wizard moved to the previous
    // step and a model reload is needed. Return false if the wizard is still on the same step
    // (e.g. the wizard is already on the first step)
    typedef bool (*WizardOnPreviousFunction)(WizardModel* model, void* state);
    WizardOnPreviousFunction onPreviousFunc = 0;
    WizardElement* onPrevious(WizardOnPreviousFunction func) {
      onPreviousFunc = func;
      return this;
    };

    // Override callback for next button. Return true if the wizard moved to the next
    // step and a model reload is needed. Return false if the wizard is still on the same step
    // (e.g. the wizard is already on the last step)
    typedef bool (*WizardOnNextFunction)(WizardModel* model, void* state);
    WizardOnNextFunction onNextFunc = 0;
    WizardElement* onNext(WizardOnNextFunction func) {
      onNextFunc = func;
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
