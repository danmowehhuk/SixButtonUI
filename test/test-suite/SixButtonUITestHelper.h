#ifndef __test_SixButtonUITestHelper_h
#define __test_SixButtonUITestHelper_h


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
    }


    void reset() {
      resetLastSelected(_ui->_nav);
      _ui->_rootElementIdx = 0;
      goToRoot();
    };

    void goToRoot() {
      goToElement(_ui->_nav->getChild(0));
    };

    bool goToNamedElement(const __FlashStringHelper* name) {
      return goToNamedElement(reinterpret_cast<const char*>(name), true);
    };

    bool goToNamedElement(const char* name, bool namePmem = false) {
      UIElement* rootElement = _ui->_nav;
      UIElement* element = nullptr;
      if (!name) {
        element = rootElement->getChild(0);
      } else {
        element = findElementWithTitle(rootElement, name, namePmem);
      }
      return goToElement(element);
    };

    bool goToElement(UIElement* element) {
      if (element) {
        _ui->goTo(element);
        if (_ui->_currWidget) {
          delete _ui->_currWidget;
          _ui->_currWidget = nullptr;
        }
        _ui->render();
        return true;
      } else {
        return false;
      }
    }

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

    UIElement* findElementWithTitle(UIElement* element, const char* title, bool titlePmem) {
      if (titleMatches(element, title, titlePmem)) {
        return element;
      } else if (element->getChildCount() > 0) {
        for (uint8_t i = 0; i < element->getChildCount(); i++) {
          UIElement* child = element->getChild(i);
          UIElement* match = findElementWithTitle(child, title, titlePmem);
          if (match != nullptr) {
            return match;
          }
        }
      }
      return nullptr;
    };

    bool titleMatches(UIElement* element, const char* title, bool titlePmem) {
      bool isMatch = false;
      if (titlePmem) {
        if (element->isTitlePmem()) {
          // both are in PROGMEM
          // For efficiency, only check pointer equality
          isMatch = (element->getTitle() == title);
          if (!isMatch) {
            // Maybe different PROGMEM strings with the same value
            // This is less efficient, but still valid
            uint8_t i = 0;
            char* elementTitle = element->getTitle();
            while (i < 20) {
              i++;
              char c1 = pgm_read_byte(elementTitle++);
              char c2 = pgm_read_byte(title++);
              if (c1 != c2) break;
              if (c1 == '\0') {
                isMatch = true;
                break;
              }
            }        
          }
        } else {
          // title in PROGMEM, element->getTitle() in regular memory
          isMatch = (strcmp_P(element->getTitle(), title) == 0);
        }
      } else {
        if (element->isTitlePmem()) {
          // title in regular memory, element->getTitle() in PROGMEM
          isMatch = (strcmp_P(title, element->getTitle()) == 0);
        } else {
          // both in regular memory
          isMatch = (strcmp(element->getTitle(), title) == 0);
        }
      }
      return isMatch;
    };

};


#endif
