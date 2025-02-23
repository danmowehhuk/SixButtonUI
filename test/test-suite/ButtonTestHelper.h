#ifndef __test_ButtonTestHelper_h
#define __test_ButtonTestHelper_h


#include <SixButtonUI.h>
#include <eventuino/Button.h>

class ButtonTestHelper {

  public:
    ButtonTestHelper(SixButtonUI* ui): _ui(ui) {};

    void reset() {
      resetLastSelected(_ui->_nav);
      UIElement* main = _ui->_nav->getChild(0);
      _ui->goTo(main);
      if (_ui->_currWidget) {
        delete _ui->_currWidget;
        _ui->_currWidget = nullptr;
      }
      _ui->render();
    }

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

  private:
    ButtonTestHelper(ButtonTestHelper &t) = delete;
    SixButtonUI* _ui;

    void pressAndRelease(Button* b) {
      Widget::Model* _currWidgetModel = _ui->widgetModel();
      if (b->onPressed != 0) b->onPressed(b->getValue(), _currWidgetModel);
      if (b->onReleased != 0) b->onReleased(b->getValue(), _currWidgetModel);
    };

    void resetLastSelected(UIElement* element) {
      for (uint8_t i = 0; i < element->getChildCount(); i++) {
        UIElement* child = element->getChild(i);
        if (child->type == UIElement::Type::SUB_MENU) {
          static_cast<SubMenuElement*>(child)->lastSelected = 0;
        } else if (child->type == UIElement::Type::SELECTOR) {
          static_cast<SelectorElement*>(child)->lastSelected = 0;
        }
        resetLastSelected(child);
      }
    };

};


#endif