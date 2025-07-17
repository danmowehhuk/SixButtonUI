#include "UIElement.h"

const UIElement* UIElement::getParent() const {
  return _parent;
}

void UIElement::setParent(UIElement* parent) {
  _parent = parent;
}

const uint8_t UIElement::getChildCount() const {
  return _numChildren;
}

const UIElement* UIElement::getChild(uint8_t index) const {
  return _children[index];
}

// Move constructor
UIElement::UIElement(UIElement&& other) noexcept
    : type(other.type),
      _children(other._children),
      _numChildren(other._numChildren),
      _title(other._title),
      // _instruction(other._instruction),
      // _footer(other._footer),
      // _isTitlePmem(other._isTitlePmem),
      // _isInstructionPmem(other._isInstructionPmem),
      // _isFooterPmem(other._isFooterPmem),
      _parent(other._parent) {

    other._children = nullptr;
    other._numChildren = 0;
    other._title = nullptr; 
    // other._instruction = nullptr; 
    // other._footer = nullptr; 

    for (uint8_t i = 0; i < _numChildren; i++) {
        if (_children[i]) {
            _children[i]->setParent(this);
        }
    }
}
