#include <Arduino.h>
#include "UIElement.h"

const UIElement* UIElement::getParent() const {
  return _parent;
}

void UIElement::setParent(UIElement* parent) {
  if (_parent == nullptr) _parent = parent;
}

const uint8_t UIElement::getChildCount() const {
  return _numChildren;
}

const UIElement* UIElement::getChild(uint8_t index) const {
  return _children[index];
}
