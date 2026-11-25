#include "UIElement.h"

UIElement* UIElement::getParent() const {
  return _parent;
}

void UIElement::setParent(UIElement* parent) {
  _parent = parent;
}

uint8_t UIElement::getChildCount() const {
  return _numChildren;
}

UIElement* UIElement::getChild(uint8_t index) const {
  return _children[index];
}

uint8_t UIElement::getChildIndex(UIElement* child) const {
  for (uint8_t i = 0; i < _numChildren; i++) {
    if (_children[i] == child) {
      return i;
    }
  }
  return 0;
}

// Move constructor
UIElement::UIElement(UIElement&& other) noexcept
    : id(other.id),
      type(other.type),
      _children(other._children),
      _numChildren(other._numChildren),
      _title(other._title),
      _instruction(other._instruction),
      _footer(other._footer),
      _enterMessage(other._enterMessage),
      _isTitlePmem(other._isTitlePmem),
      _isInstructionPmem(other._isInstructionPmem),
      _isFooterPmem(other._isFooterPmem),
      _isEnterMessagePmem(other._isEnterMessagePmem),
      _parent(other._parent),
      _hasId(other._hasId),
      _isHidden(other._isHidden) {

    other._children = nullptr;
    other._numChildren = 0;
    other._title = nullptr; 
    other._instruction = nullptr; 
    other._footer = nullptr; 
    other._enterMessage = nullptr;

    for (uint8_t i = 0; i < _numChildren; i++) {
        if (_children[i]) {
            _children[i]->setParent(this);
        }
    }
}
