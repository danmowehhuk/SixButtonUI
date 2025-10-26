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

    // The nav config may contain multiple root elements,
    // so this keeps track of which one we're on
    uint8_t _rootElementIdx;

    // The config and widget currently being displayed.
    UIElement* _currConfig;
    Widget* _currWidget = nullptr;

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

namespace sixbuttonui {

  inline SelectorElement*  selector()   { return new SelectorElement();   };
  inline SubMenuElement*   subMenu()    { return new SubMenuElement();    };
  inline TextInputElement* textInput()  { return new TextInputElement();  };
  inline ComboBoxElement*  comboBox()   { return new ComboBoxElement();   };
  inline WizardElement*    wizard()     { return new WizardElement();     };

};


#endif