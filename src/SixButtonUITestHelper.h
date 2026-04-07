#ifndef _sixbuttonui_SixButtonUITestHelper_h
#define _sixbuttonui_SixButtonUITestHelper_h


#include <Arduino.h>
#include <SixButtonUI.h>

class SixButtonUITestHelper {

  public:
    SixButtonUITestHelper(SixButtonUI* ui): _ui(ui) {};

    void pressAndReleaseUp() {
      pressAndRelease(&(_ui->_up));
    };
    void pressAndReleaseDown() {
      pressAndRelease(&(_ui->_down));
    };
    void pressAndReleaseLeft() {
      pressAndRelease(&(_ui->_left));
    };
    void pressAndReleaseRight() {
      pressAndRelease(&(_ui->_right));
    };
    void pressAndReleaseMenuBack() {
      pressAndRelease(&(_ui->_menuBack));
    };
    void pressAndReleaseSelectEnter() {
      pressAndRelease(&(_ui->_selectEnter));
    };
    void longPressLeft() {
      longPress(&(_ui->_left));
    };
    void longPressRight() {
      longPress(&(_ui->_right));
    };

    void reset() {
      resetLastSelected(_ui->_nav);
      _ui->_rootElementIdx = 0;
      goToRoot();
    };

    void goToRoot() {
      goToElement(_ui->_nav->getChild(0));
    };

    bool goToElementById(uint8_t id) {
      return goToElement(_ui->findElementById(id));
    };

    void setState(void* state) {
      _ui->_state = state;
    };

    void* getState() {
      return _ui->_state;
    };

    WidgetModel* getWidgetModel() {
      return _ui->widgetModel();
    };

    bool goToElement(UIElement* element) {
      if (element) {
        _ui->setNext(element);
        if (_ui->_currWidget) {
          delete _ui->_currWidget;
          _ui->_currWidget = nullptr;
        }
        _ui->render();
        return true;
      } else {
        return false;
      }
    };

    // disable move and copy
    SixButtonUITestHelper(SixButtonUITestHelper&& other) = delete;
    SixButtonUITestHelper& operator=(SixButtonUITestHelper&& other) = delete;
    SixButtonUITestHelper(const SixButtonUITestHelper&) = delete;
    SixButtonUITestHelper& operator=(const SixButtonUITestHelper&) = delete;

  private:
    SixButtonUI* _ui;

    void pressAndRelease(Button* b) {
      WidgetModel* _currWidgetModel = _ui->widgetModel();
      if (b->onPressed != 0) b->onPressed(b->getValue(), _currWidgetModel);
      if (b->onReleased != 0) b->onReleased(b->getValue(), _currWidgetModel);
    };

    void longPress(Button* b) {
      WidgetModel* _currWidgetModel = _ui->widgetModel();
      if (b->onLongPress != 0) b->onLongPress(b->getValue(), _currWidgetModel);
    };

    void resetLastSelected(UIElement* element) {
      for (uint8_t i = 0; i < element->getChildCount(); i++) {
        UIElement* child = element->getChild(i);
        if (child->type == UIElement::Type::SUB_MENU) {
          static_cast<SubMenuElement*>(child)->lastSelected = 0;
        }
        resetLastSelected(child);
      }
    };

};


#endif
