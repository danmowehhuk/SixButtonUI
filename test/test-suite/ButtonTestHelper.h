#ifndef __test_ButtonTestHelper_h
#define __test_ButtonTestHelper_h


#include <SixButtonUI.h>
#include <eventuino/Button.h>

class ButtonTestHelper {

  public:
    ButtonTestHelper(SixButtonUI* ui): _ui(ui) {};

    void goHome() {
      UIElement* main = _ui->_nav->getChild(0);
      if (main->type == UIElement::Type::SUB_MENU) {
        static_cast<SubMenuElement*>(main)->lastSelected = 0;
      } else if (main->type == UIElement::Type::SELECTOR) {
        static_cast<SelectorElement*>(main)->lastSelected = 0;
      }
      _ui->goTo(main);
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
      Widget::Model* _currWidgetModel = _ui->_currWidgetModel;
      if (b->onPressed != 0) b->onPressed(b->getValue(), _currWidgetModel);
      if (b->onReleased != 0) b->onReleased(b->getValue(), _currWidgetModel);
    };

};


#endif