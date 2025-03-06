#ifndef _sixbuttonui_SubMenuElement_h
#define _sixbuttonui_SubMenuElement_h


#include "UIElement.h"

/*
 * SubMenuElement is a pure navigation element. It looks like a selector, but the
 * values come from the navigation config, and the action is always to 
 * "enter" the selected node of the navigation tree. Therefore, it does not
 * take a model loader function or action function.
 */
class SubMenuElement: public UIElementBase<SubMenuElement> {

  public:
    SubMenuElement(): UIElementBase(SUB_MENU) {};

    template <typename... Args>
    SubMenuElement* withMenuItems(UIElement* childElement, Args... moreChildElements) {
      return withChildren(childElement, moreChildElements...);
    };

    /*
     * The selection options for a sub-menu come from the nav config and cannot
     * be changed. The title defaults to what's provided in the nav config, but
     * it can be overridden.
     */
    struct Model {
      char* title = nullptr;
      __FlashStringHelper* titlePmem = nullptr;
      ~Model() {
        title = nullptr;
        titlePmem = nullptr;
      };
    };
    typedef void (*ModelFunction)(Model* model, void* state);
    ModelFunction modelLoader = 0;
    SubMenuElement* withModelFunction(ModelFunction modelFunction) {
      modelLoader = modelFunction;
      return this;
    };

    uint8_t lastSelected = 0;

};


#endif