#ifndef _sixbuttonui_SixButtonUI_h
#define _sixbuttonui_SixButtonUI_h


#include <Eventuino.h>
#include <eventuino/Button.h>
#include "sixbuttonui/ComboBoxElement.h"
#include "sixbuttonui/NavigationConfig.h"
#include "sixbuttonui/SelectorElement.h"
#include "sixbuttonui/SubMenuElement.h"
#include "sixbuttonui/TextInputElement.h"
#include "sixbuttonui/UIElement.h"
#include "sixbuttonui/ViewModel.h"
#include "sixbuttonui/Widget.h"
#include "sixbuttonui/WizardElement.h"

using namespace eventuino;

class SixButtonUI: public EventSource {

  public:
    typedef void (*RenderFunction)(ViewModel viewModel);
    SixButtonUI(uint8_t upButtonPin, uint8_t downButtonPin, uint8_t leftButtonPin,
                uint8_t rightButtonPin, uint8_t menuBackButtonPin, uint8_t enterSelectButtonPin,
                RenderFunction renderFunction, NavigationConfig* navConfig);

    // Tells SixButtonUI to switch to a different element in the nav config
    void goTo(UIElement* element);
    void goTo(uint8_t id);

    // Reload the current widget
    void reload();

    // If the widget's onEnter function doesn't define what UI element to load next,
    // the default behavior is to load the parent element, or if the parent is the root,
    // reload the current element
    void goToDefault();

    // Required by EventSource
    void setup() override;
    void poll(void* state = nullptr) override;

  private:
    Button _down;
    Button _up;
    Button _left;
    Button _right;
    Button _menuBack;
    Button _selectEnter;
    NavigationConfig* _nav;
    RenderFunction _renderFunction;

    // When menu/back button is pressed, return to the parent node in the 
    // nav config. If the parent is the NavigationConfig object itself, 
    // switch to the next child instead.
    void menuBack();

    // Find an element by its id
    UIElement* findElementById(uint8_t id);
    UIElement* findElementByIdRecursive(UIElement* element, uint8_t id);

    // The nav config may contain multiple root elements,
    // so this keeps track of which one we're on
    uint8_t _rootElementIdx;

    // The config and widget currently being displayed.
    UIElement* _currConfig;
    Widget* _currWidget = nullptr;

    // Destroy and reload the widget and its state on the next render cycle
    bool _forceReloadWidget = false;

    // The state object and SixButtonUI itself are stored to be made 
    // available to the widget model for use in button event handlers.
    void* _state = nullptr;
    static SixButtonUI* UI(void* widgetModel);

    // Render-cycle functions
    void render();
    void clearHandlers();
    void maybeInitWidget();
    Widget* newForType(UIElement::Type type);
  
    // Allow the test helper to access the widget model and mimic
    // pressing buttons.
    WidgetModel* widgetModel();
    friend class SixButtonUITestHelper;

};

// AVR linker symbols for heap tracking (must be declared outside namespace)
#if defined(DEBUG)
extern char __heap_start, *__brkval;
#endif

namespace sixbuttonui {

  inline SelectorElement*  selector()   { return new SelectorElement();   };
  inline SelectorElement*  selector(uint8_t id) { return new SelectorElement(id); };
  inline SubMenuElement*   subMenu()    { return new SubMenuElement();    };
  inline SubMenuElement*   subMenu(uint8_t id) { return new SubMenuElement(id); };
  inline TextInputElement* textInput()  { return new TextInputElement();  };
  inline TextInputElement* textInput(uint8_t id) { return new TextInputElement(id); };
  inline ComboBoxElement*  comboBox()   { return new ComboBoxElement();   };
  inline ComboBoxElement*  comboBox(uint8_t id) { return new ComboBoxElement(id); };
  inline WizardElement*    wizard()     { return new WizardElement();     };
  inline WizardElement*    wizard(uint8_t id) { return new WizardElement(id); };

#if defined(DEBUG)
  // Free memory debugging function
  // Uses AVR-specific heap variables to calculate available RAM
  inline int freeMemory() {
    char top;
    if (__brkval == 0) {
      return &top - &__heap_start;
    } else {
      return &top - __brkval;
    }
  };
#endif
};


#endif