#ifndef _sixbuttonui_SelectorElement_h
#define _sixbuttonui_SelectorElement_h


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
 */
class SelectorElement: public UIElementBase<SelectorElement> {

  public:
    SelectorElement(): UIElementBase(SELECTOR) {};

    struct Model {
      // char* title = nullptr;
      // char* instruction = nullptr;
      char** optionNames = nullptr;
      __FlashStringHelper** optionNamesPmem = nullptr;
      size_t* optionNameLengths = nullptr;
      char** optionValues = nullptr;
      __FlashStringHelper** optionValuesPmem = nullptr;
      size_t* optionValueLengths = nullptr;
      char* currValue = nullptr;
      uint8_t numOptions = 0;
      uint8_t currIndex = 0;
      ~Model() {
        // if (optionNames) {
        //   for (uint8_t i = 0; i < numOptions; i++) {
        //     optionNames[i] = nullptr;
        //   }
        //   // delete[] optionNames;
        //   optionNames = nullptr;
        // }
        // if (optionNamesPmem) {
        //   for (uint8_t i = 0; i < numOptions; i++) {
        //     optionNamesPmem[i] = nullptr;
        //   }
        //   // delete[] optionNamesPmem;
        //   optionNamesPmem = nullptr;
        // }
        // // title = nullptr;
        // // if (optionNames) delete optionNames;
        // // if (optionValues) delete optionValues;
      };
    };
    typedef void (*ModelFunction)(Model* model, void* state);
    ModelFunction modelLoader = 0;
    SelectorElement* withModelFunction(ModelFunction modelFunction) {
      modelLoader = modelFunction;
      return this;
    };

    typedef void* (*OnEnterFunction)(char* selectionName, char* selectionValue, void* state);
    OnEnterFunction onEnterFunc = 0;
    SelectorElement* onEnter(OnEnterFunction func) {
      onEnterFunc = func;
      return this;
    };

    uint8_t lastSelected = 0;

  private:
    SelectorElement(SelectorElement &t) = delete;

};


#endif