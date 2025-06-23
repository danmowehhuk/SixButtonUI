#ifndef _sixbuttonui_DummyElement_h
#define _sixbuttonui_DummyElement_h


#include <sixbuttonui/UIElement.h>

class DummyElement: public UIElementBase<DummyElement> {

  public:
    DummyElement(): UIElementBase(UIElement::UNDEFINED) {};


  private:
    DummyElement(DummyElement &t) = delete;

};


#endif