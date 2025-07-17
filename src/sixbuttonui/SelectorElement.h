#ifndef _sixbuttonui_SelectorElement_h
#define _sixbuttonui_SelectorElement_h


#include "SelectorModel.h"
#include "UIElement.h"

/*
 * SelectorElement:
 *
 * Convenience function:       sixbuttonui::selector()
 *
 * Methods:
 *    withTitle(...)            Shown in navigation menus
 *
 *    withInstruction(...)      Displayed above selections, default is
 *                              to use the title
 *
 *    withModelFunction(...)    A function that loads the selector options.
 *                              Takes the SelectorElement::Model and a void*
 *                              as parameters and returns void. The void*
 *                              param points to an arbitrary state object
 *                              of the user's design
 *
 *    onEnter(...)              The function to execute when a selection
 *                              is made. Takes the name and value of the
 *                              selection as well as the same void* state
 *                              object that was passed to the model loader.
 *                              Returns a void* to the same or a new 
 *                              arbitrary state object that will be passed
 *                              to the next model loader.
 *
 *    withFooter(...)           Displayed at the bottom of the display
 */
class SelectorElement: public UIElementBase<SelectorElement> {

  public:
    SelectorElement(): UIElementBase(UIElement::Type::SELECTOR) {};

    typedef void (*SelectorModelFunction)(SelectorModel* model, void* state);
    SelectorModelFunction modelLoader = 0;
    SelectorElement* withModelFunction(SelectorModelFunction modelFunction) {
      modelLoader = modelFunction;
      return this;
    };

    typedef void* (*SelectorOnEnterFunction)(char* selectionName, bool namePmem, char* selectionValue, bool valuePmem, void* state);
    SelectorOnEnterFunction onEnterFunc = 0;
    SelectorElement* onEnter(SelectorOnEnterFunction func) {
      onEnterFunc = func;
      return this;
    };

    // Disable moving and copying
    SelectorElement(SelectorElement&& other) = delete;
    SelectorElement& operator=(SelectorElement&& other) = delete;
    SelectorElement(const SelectorElement&) = delete;
    SelectorElement& operator=(const SelectorElement&) = delete;

  protected:
    SelectorElement(UIElement::Type type): UIElementBase(type) {};

};


#endif
