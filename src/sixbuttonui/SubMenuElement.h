#ifndef _sixbuttonui_SubMenuElement_h
#define _sixbuttonui_SubMenuElement_h


#include "SelectorElement.h"
#include <Arduino.h>

/*
 * SubMenuElement is a pure navigation element. It has type SELECTOR for rendering, 
 * but the values come from the navigation config, and the action is always to 
 * "enter" the selected node of the navigation tree. Therefore, it does not
 * take an onEnter function.
 *
 * Note that the with* methods are overridden to return SubMenuElement*
 * so that the methods can be chained.
 *
 * The selection options for a sub-menu come from the nav config and cannot
 * be changed. The title defaults to what's provided in the nav config, but
 * it can be overridden by the inherited SelectorModelFunction. Updates to any 
 * other model properties than the title are ignored.  
 */
class SubMenuElement: public SelectorElement {

  public:
    SubMenuElement(): SelectorElement(UIElement::Type::SUB_MENU) {};
    SubMenuElement(uint8_t id): SelectorElement(UIElement::Type::SUB_MENU, id) {};

    template <typename... Args>
    SubMenuElement* withMenuItems(UIElement* childElement, Args... moreChildElements) {
      return static_cast<SubMenuElement*>(withChildren(childElement, moreChildElements...));
    };

    SubMenuElement* withTitle(const char* title, bool pmem = false) {
      SelectorElement::withTitle(title, pmem);
      return this;
    };

    SubMenuElement* withTitle(const __FlashStringHelper* title) {
      SelectorElement::withTitle(title);
      return this;
    };

    SubMenuElement* withInstruction(const char* instruction, bool pmem = false) {
      SelectorElement::withInstruction(instruction, pmem);
      return this;
    };

    SubMenuElement* withInstruction(const __FlashStringHelper* instruction) {
      SelectorElement::withInstruction(instruction);
      return this;
    };

    SubMenuElement* withFooter(const char* footer, bool pmem = false) {
      SelectorElement::withFooter(footer, pmem);
      return this;
    };

    SubMenuElement* withFooter(const __FlashStringHelper* footer) {
      SelectorElement::withFooter(footer);
      return this;
    };

    SubMenuElement* setHidden(bool hidden) {
      SelectorElement::setHidden(hidden);
      return this;
    };

    SubMenuElement* withModelFunction(SelectorModelFunction modelFunction) {
      SelectorElement::withModelFunction(modelFunction);
      return this;
    };

    SelectorElement* onEnter(SelectorOnEnterFunction func) = delete;

    uint8_t lastSelected = 0;

};


#endif
