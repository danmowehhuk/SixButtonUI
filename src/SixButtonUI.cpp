#include "SixButtonUI.h"
#include "sixbuttonui/NavigationConfig.h"
#include "sixbuttonui/Widget.h"
#include "sixbuttonui/SelectorWidget.h"
#include "sixbuttonui/SubMenuWidget.h"

int freeMemory();

SixButtonUI::SixButtonUI(
      uint8_t upButtonPin, uint8_t downButtonPin, uint8_t leftButtonPin,
      uint8_t rightButtonPin, uint8_t menuBackButtonPin, uint8_t enterSelectButtonPin,
      const NavigationConfig* navConfig, RenderFunction renderFunction):
          _up(upButtonPin, 0), 
          _down(downButtonPin, 0), 
          _left(leftButtonPin, 0),
          _right(rightButtonPin, 0),
          _menuBack(menuBackButtonPin, 0),
          _selectEnter(enterSelectButtonPin, 0),
          _nav(navConfig),
          _rootElementIdx(0),
          _currConfig(navConfig->getChild(_rootElementIdx)),
          _renderFunction(renderFunction) {};

void SixButtonUI::setup() {
  _down.setup();
  _up.setup();
  _left.setup();
  _right.setup();
  _menuBack.setup();
  _selectEnter.setup();
  render();
}

void SixButtonUI::poll(void* state = nullptr) {
  /*
   * The state object (if any) sent from Eventuino is not used.
   */ 
  _down.poll(_currWidget->widgetModel());
  _up.poll(_currWidget->widgetModel());
  _left.poll(_currWidget->widgetModel());
  _right.poll(_currWidget->widgetModel());
  _menuBack.poll(_currWidget->widgetModel());
  _selectEnter.poll(_currWidget->widgetModel());
}

void SixButtonUI::render() {
  clearHandlers();
  maybeInitWidget();
  _currWidget->initWidgetModel(_state);
  _currWidget->widgetModel()->ui = this;
  _currWidget->widgetModel()->state = _state;
  _currWidget->widgetModel()->element = _currConfig;
  ViewModel vm;
  _currWidget->updateViewModel(&vm);
  _renderFunction(&vm);

  _up.onPressed = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onUpPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _down.onPressed = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onDownPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _left.onPressed = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onLeftPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _right.onPressed = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onRightPressed(value, widgetModel);
    UI(widgetModel)->render();
  };

  /*
   * selectEnter and menuBack are "onReleased" to avoid unwanted
   * actions when trying to long-press both together
   */
  _selectEnter.onReleased = [](uint8_t value, void* widgetModel) {
    void* stateIn = static_cast<Widget::Model*>(widgetModel)->state;
    void* stateOut = UI(widgetModel)->_currWidget->onEnter(value, widgetModel, stateIn);
    UI(widgetModel)->_state = stateOut;
    UI(widgetModel)->render();
  };
  _menuBack.onReleased = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->menuBack();
    UI(widgetModel)->render();
  };
}

void SixButtonUI::goTo(UIElement* element) {
  _currConfig = element;
}

void SixButtonUI::menuBack() {
  UIElement* parent = _currConfig->getParent();
  if (parent->type == UIElement::Type::ROOT) {
    if (_rootElementIdx < parent->getChildCount() - 1) {
      _rootElementIdx++;
    } else {
      _rootElementIdx = 0;
    }
    if (_currConfig->type == UIElement::Type::SUB_MENU) {
      static_cast<SubMenuElement*>(_currConfig)->lastSelected = 
          static_cast<SubMenuWidget::SubMenuModel*>(_currWidget->widgetModel())->currIndex;
    } else if (_currConfig->type == UIElement::Type::SELECTOR) {
      static_cast<SelectorElement*>(_currConfig)->lastSelected = 
          static_cast<SelectorWidget::SelectorModel*>(_currWidget->widgetModel())->currIndex;
    }
    _currConfig = parent->getChild(_rootElementIdx);
  } else {
    if (_currConfig->type == UIElement::Type::SUB_MENU) {
      static_cast<SubMenuElement*>(_currConfig)->lastSelected = 0;
    } else if (_currConfig->type == UIElement::Type::SELECTOR) {
      static_cast<SelectorElement*>(_currConfig)->lastSelected = 0;
    }
    goTo(parent);
  }
}

void SixButtonUI::clearHandlers() {
  _up.onPressed = 0;
  _down.onPressed = 0;
  _left.onPressed = 0;
  _right.onPressed = 0;
  _menuBack.onReleased = 0;
  _selectEnter.onReleased = 0;
}

void SixButtonUI::maybeInitWidget() {
  if (!_currWidget || _currWidget->wConf != _currConfig) {
    if (_currWidget) delete _currWidget;
    _currWidget = newForType(_currConfig->type);
  }
}

Widget* SixButtonUI::newForType(UIElement::Type type) {
  Widget* out = nullptr;
  switch (type) {
    case UIElement::Type::SUB_MENU:
      out = new SubMenuWidget(static_cast<SubMenuElement*>(_currConfig));
      break;
    case UIElement::Type::SELECTOR:
      out = new SelectorWidget(static_cast<SelectorElement*>(_currConfig));
      break;
    // case UIElement::Type::TEXT_INPUT:

    //   break;
    // case UIElement::Type::COMBO_BOX:

    //   break;
    // case UIElement::Type::WIZARD:

    //   break;
    default:
#if (defined(DEBUG))
      Serial.println(String(F("ERROR: Invalid widget type: ")) + String(_currConfig->type));
      delay(100);
#endif
      break;
  }
  return out;
}

Widget::Model* SixButtonUI::widgetModel() {
  if (_currWidget) return _currWidget->widgetModel();
  return nullptr;
}

SixButtonUI* SixButtonUI::UI(void* widgetModel) {
  return static_cast<Widget::Model*>(widgetModel)->ui;
}

