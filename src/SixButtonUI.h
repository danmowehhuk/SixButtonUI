#ifndef _SixButtonUI_h
#define _SixButtonUI_h


#include <Arduino.h>
#include <Eventuino.h>
#include <eventuino/Button.h>
#include "sixbuttonui/UIElement.h"
#include "sixbuttonui/NavigationConfig.h"
#include "sixbuttonui/SubMenuElement.h"
#include "sixbuttonui/SelectorElement.h"
#include "sixbuttonui/Widget.h"

using namespace eventuino;

class SixButtonUI: public EventSource {

  public:
    typedef void (*RenderFunction)(ViewModel viewModel);
    SixButtonUI(uint8_t upButtonPin, uint8_t downButtonPin, uint8_t leftButtonPin,
              uint8_t rightButtonPin, uint8_t menuBackButtonPin, uint8_t enterSelectButtonPin,
              const NavigationConfig* navConfig, RenderFunction renderFunction);

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
    uint8_t _rootElementIdx;
    UIElement* _currConfig;
    Widget* _currWidget = nullptr;
    RenderFunction _renderFunction;
    void render();
    void clearHandlers();
    void maybeInitWidget();
    void menuBack();
    Widget* newForType(UIElement::Type type);
    static SixButtonUI* UI(void* widgetModel);

    void* _state = nullptr;

    // Allows the test suite to mimic pressing buttons
    WidgetModel* widgetModel();
    friend class ButtonTestHelper;
};

namespace sixbuttonui {

  inline SelectorElement* selector() { return new SelectorElement(); };
  inline SubMenuElement*  subMenu()  { return new SubMenuElement();  };

}


#endif