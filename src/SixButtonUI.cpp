#include "SixButtonUI.h"
#include "sixbuttonui/ComboBoxWidget.h"
#include "sixbuttonui/SelectorWidget.h"
#include "sixbuttonui/SubMenuWidget.h"
#include "sixbuttonui/TextInputWidget.h"
#include "sixbuttonui/ViewModel.h"
#include "sixbuttonui/WidgetModel.h"
#include "sixbuttonui/WizardWidget.h"
#include "sixbuttonui/PopupWidget.h"

using namespace eventuino;

SixButtonUI::SixButtonUI(
      uint8_t upButtonPin, uint8_t downButtonPin, uint8_t leftButtonPin,
      uint8_t rightButtonPin, uint8_t menuBackButtonPin, uint8_t enterSelectButtonPin,
      RenderFunction renderFunction, NavigationConfig* navConfig):
          _up(upButtonPin, 0), 
          _down(downButtonPin, 0), 
          _left(leftButtonPin, 0),
          _right(rightButtonPin, 0),
          _menuBack(menuBackButtonPin, 0),
          _selectEnter(enterSelectButtonPin, 0),
          _timer(0),
          _nav(navConfig),
          _rootElementIdx(0),
          _currConfig(_nav->getChild(_rootElementIdx)),
          _renderFunction(renderFunction) {};

void SixButtonUI::setup() {
  _down.setup();
  _up.setup();
  _left.setup();
  _right.setup();
  _menuBack.setup();
  _selectEnter.setup();
  _timer.setup();
  render();
}

void SixButtonUI::poll(void* state) {
  /*
   * The state object (if any) sent from Eventuino is not used. Instead,
   * the current widget's own model is passed so it can be made available
   * within the event handler callbacks (e.g. onPressed, onReleased).
   */ 
  if (_currWidget->getModel()) {
    _down.poll(_currWidget->getModel());
    _up.poll(_currWidget->getModel());
    _left.poll(_currWidget->getModel());
    _right.poll(_currWidget->getModel());
    _menuBack.poll(_currWidget->getModel());
    _selectEnter.poll(_currWidget->getModel());
    _timer.poll(_currWidget->getModel());
  }
}

void SixButtonUI::render() {

  clearHandlers();
  _oldCurrConfig = nullptr;

  // If we've moved to a different UIElement, clean up the old state
  // and instantiate the new _currWidget.
  maybeInitWidget();

  if (!_currWidget) {
    // No widget was created due to an error. Return without attaching
    // any button handlers so that although SixButtonUI becomes  
    // unresponsive, other Eventuino EventSources keep functioning.
    return;
  }

  _currWidget->populateModel(_state);

  // Make the state and this SixButtonUI instance available in
  // the widget model.
  _currWidget->getModel()->_state = _state;
  _currWidget->getModel()->_ui = this;

  // Update the ViewModel based on the current widget model
  _renderFunction(_currWidget->getViewModel());

  // Re-apply all the button action handlers
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
  _up.onLongPress = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onUpLongPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _up.enableRepeat(_currWidget->onUpLongPressRepeat());
  _down.onLongPress = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onDownLongPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _down.enableRepeat(_currWidget->onDownLongPressRepeat());
  _left.onLongPress = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onLeftLongPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _left.enableRepeat(_currWidget->onLeftLongPressRepeat());
  _right.onLongPress = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->_currWidget->onRightLongPressed(value, widgetModel);
    UI(widgetModel)->render();
  };
  _right.enableRepeat(_currWidget->onRightLongPressRepeat());

   // selectEnter and menuBack are "onReleased" to avoid unwanted
   // actions when trying to long-press both together
  _selectEnter.onReleased = [](uint8_t value, void* widgetModel) {
    void* stateIn = static_cast<WidgetModel*>(widgetModel)->_state;
    void* stateOut = UI(widgetModel)->_currWidget->onEnter(value, widgetModel, stateIn);
    UI(widgetModel)->_state = stateOut;
    UI(widgetModel)->render();
  };
  _menuBack.onReleased = [](uint8_t value, void* widgetModel) {
    UI(widgetModel)->menuBack();
    UI(widgetModel)->render();
  };

  _timer.onExpire = [](uint8_t value, void* widgetModel) {
    void* stateIn = static_cast<WidgetModel*>(widgetModel)->_state;
    void* stateOut = UI(widgetModel)->_currWidget->onTimerExpired(value, widgetModel, stateIn);
    UI(widgetModel)->_state = stateOut;
    UI(widgetModel)->render();
  };

#if (defined(DEBUG_6BUI_MEM))
  Serial.print(F("SixButtonUI.h: Free memory after render: "));
  Serial.println(sixbuttonui::freeMemory());
#endif
}

void SixButtonUI::setNext(UIElement* element) {
  if (_oldCurrConfig == nullptr) {
    // Multiple calls to setNext should not overwrite the initial value
    _oldCurrConfig = _currConfig;
  }
  _currConfig = element;
}

void SixButtonUI::setNext(uint8_t id) {
  UIElement* element = findElementById(id);
  if (element->getParent()->type == UIElement::Type::ROOT) {
    _rootElementIdx = element->getParent()->getChildIndex(element);
  }
  setNext(element);
}

void SixButtonUI::setNextDefault() {
  if (_currConfig->getParent()->type == UIElement::Type::ROOT) {
    reload();
  } else {
    setNext(const_cast<UIElement*>(_currConfig->getParent()));
  }
}

void SixButtonUI::unsetNext() {
  if (_oldCurrConfig != nullptr) {
    _currConfig = _oldCurrConfig;
    _oldCurrConfig = nullptr;
  }
  _forceReloadWidget = false;
}

void SixButtonUI::reload() {
  // Force SixButtonUI to reload the current widget
  _forceReloadWidget = true;
}

void SixButtonUI::showPopup(PopupWidget::Type type, const __FlashStringHelper* message) {
  showPopup(type, reinterpret_cast<const char*>(message), true);
}

void SixButtonUI::showPopup(PopupWidget::Type type, const char* message, bool pmem = false) {
  showPopup(type, 1000, message, pmem);
}

void SixButtonUI::showPopup(PopupWidget::Type type, uint16_t duration, const __FlashStringHelper* message) {
  showPopup(type, duration, reinterpret_cast<const char*>(message), true);
}

void SixButtonUI::showPopup(PopupWidget::Type type, uint16_t duration, const char* message, bool pmem = false) {

  // Override any previous setNext() calls. Dismissing the popup will always restore the last UI state.
  unsetNext();

  PopupWidget::PopupElement* popupElement = PopupWidget::getPopupElement();
  popupElement->setPopupType(type);
  popupElement->setMessage(message, pmem);

  // Store references to the current widget and state so we can restore it after the popup is dismissed
  popupElement->setReturnTo(_currConfig, _currWidget, _state);
  _currWidget = nullptr;
  _state = nullptr;

  _menuBackDisabled = true;
  if (type == PopupWidget::Type::TIMER_CANCELABLE) {
    _timer.cancel();
    _timer.start(duration);
  }
  setNext(popupElement);
}

void SixButtonUI::dismissPopup() {
  _menuBackDisabled = false;
  if (_currConfig->type != UIElement::Type::POPUP) {
    return;
  }
  _timer.cancel();
  
  // Save the pointers before deleting the widget
  PopupWidget::PopupElement* popupElement = PopupWidget::getPopupElement();  
  UIElement* savedConfig = popupElement->getReturnToConfig();
  Widget* savedWidget = popupElement->getReturnToWidget();
  void* savedState = popupElement->getReturnToState();
  
  // Now delete the widget, which will clear the popup element
  if (_currWidget) delete _currWidget;
  
  // Restore the previous UI state
  _currConfig = savedConfig;
  _currWidget = savedWidget;
  _state = savedState;
}

void SixButtonUI::menuBack() {
  if (_menuBackDisabled) {
    return;
  }
  const UIElement* parent = _currConfig->getParent();
  if (parent->type == UIElement::Type::ROOT) {

    // Parent element is the ROOT node (NavigationConfig). Go to
    // the next child element, not the parent. Skip hidden elements.
    uint8_t childCount = parent->getChildCount();
    UIElement* nextConfig = nullptr;
    uint8_t startIdx = _rootElementIdx;
    uint8_t checkedCount = 0;
    
    // Search forward from the next index, wrapping around if needed
    do {
      if (startIdx < childCount - 1) {
        startIdx++;
      } else {
        startIdx = 0;
      }
      UIElement* candidate = const_cast<UIElement*>(parent->getChild(startIdx));
      if (!candidate->isHidden()) {
        nextConfig = candidate;
        _rootElementIdx = startIdx;
        break;
      }
      checkedCount++;
    } while (checkedCount < childCount);

    // Store the selected index of the current sub-menu so we can 
    // keep it selected while toggling through the root menus.
    if (_currConfig->type == UIElement::Type::SUB_MENU) {
      static_cast<SubMenuElement*>(_currConfig)->lastSelected = 
          static_cast<SelectorModel*>(_currWidget->getModel())->getCurrIndex();
    }

    // Switch to the next child element
    _currConfig = nextConfig;

  } else {

    // Reset the last selected value when backing out of a submenu
    if (_currConfig->type == UIElement::Type::SUB_MENU) {
      static_cast<SubMenuElement*>(_currConfig)->lastSelected = 0;
    }

    // Switch to the parent element
    setNext(const_cast<UIElement*>(parent));
  }
}

void SixButtonUI::clearHandlers() {
  _up.onPressed = 0;
  _up.onLongPress = 0;
  _down.onPressed = 0;
  _down.onLongPress = 0;
  _left.onPressed = 0;
  _left.onLongPress = 0;
  _right.onPressed = 0;
  _right.onLongPress = 0;
  _menuBack.onReleased = 0;
  _selectEnter.onReleased = 0;
  _up.enableRepeat(false);
  _down.enableRepeat(false);
  _left.enableRepeat(false);
  _right.enableRepeat(false);
}

void SixButtonUI::maybeInitWidget() {
  if (_forceReloadWidget || !_currWidget || _currWidget->_wConf != _currConfig) {
    _forceReloadWidget = false; // reset the flag
    if (_currWidget) delete _currWidget;
    if (!_currConfig) {
      return;
    }
    _currWidget = newForType(_currConfig->type);
  }
}

Widget* SixButtonUI::newForType(UIElement::Type type) {
  Widget* out = nullptr;
  switch (type) {
    case UIElement::Type::SELECTOR:
      out = new SelectorWidget(static_cast<const SelectorElement*>(_currConfig));
      break;
    case UIElement::Type::SUB_MENU:
      out = new SubMenuWidget(static_cast<const SubMenuElement*>(_currConfig));
      break;
    case UIElement::Type::TEXT_INPUT:
      out = new TextInputWidget(static_cast<TextInputElement*>(_currConfig));
      break;
    case UIElement::Type::COMBO_BOX:
      out = new ComboBoxWidget(static_cast<ComboBoxElement*>(_currConfig));
      break;
    case UIElement::Type::WIZARD:
      out = new WizardWidget(static_cast<WizardElement*>(_currConfig));
      break;
    case UIElement::Type::POPUP:
      out = new PopupWidget(static_cast<PopupWidget::PopupElement*>(_currConfig));
      break;
    default:
#if (defined(DEBUG))
      Serial.print(F("ERROR: Invalid widget type: "));
      Serial.println(_currConfig->type);
      delay(100);
#endif
      break;
  }
  return out;
}

UIElement* SixButtonUI::findElementById(uint8_t id) {
  if (!_nav) {
    return nullptr;
  }
  return findElementByIdRecursive(static_cast<UIElement*>(_nav), id);
}

UIElement* SixButtonUI::findElementByIdRecursive(UIElement* element, uint8_t id) {
  if (!element) {
    return nullptr;
  }
  
  // Check if current element matches (only if it has an id set)
  if (element->hasId() && element->id == id) {
    return element;
  }
  
  // Recursively search children
  uint8_t childCount = element->getChildCount();
  for (uint8_t i = 0; i < childCount; i++) {
    UIElement* child = const_cast<UIElement*>(element->getChild(i));
    UIElement* found = findElementByIdRecursive(child, id);
    if (found) {
      return found;
    }
  }
  
  return nullptr;
}

WidgetModel* SixButtonUI::widgetModel() {
  if (_currWidget) return _currWidget->getModel();
  return nullptr;
}

SixButtonUI* SixButtonUI::UI(void* widgetModel) {
  return static_cast<WidgetModel*>(widgetModel)->_ui;
}
