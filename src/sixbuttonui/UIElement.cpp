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

bool UIElement::isTitlePmem() {
  return (!_title && _titlePmem);
}

const __FlashStringHelper* UIElement::getTitle_P() const {
  return _titlePmem;
}

const char* UIElement::getTitle() const {
  return _title;
}

size_t UIElement::getTitleLength() const {
  if (_title) {
    return strlen(_title);
  } else if (_titlePmem) {
    return strlen_P(reinterpret_cast<const char*>(_titlePmem));
  }
  return 0;
}

bool UIElement::isInstructionPmem() {
  return (!_instruction && _instructionPmem);
}

const __FlashStringHelper* UIElement::getInstruction_P() const {
  return _instructionPmem;
}

const char* UIElement::getInstruction() const {
  return _instruction;
}

size_t UIElement::getInstructionLength() const {
  if (_instruction) {
    return strlen(_instruction);
  } else if (_instructionPmem) {
    return strlen_P(reinterpret_cast<const char*>(_instructionPmem));
  }
  return 0;
}

bool UIElement::isFooterPmem() {
  return (!_footer && _footerPmem);
}

const __FlashStringHelper* UIElement::getFooter_P() const {
  return _footerPmem;
}

const char* UIElement::getFooter() const {
  return _footer;
}

size_t UIElement::getFooterLength() const {
  if (_footer) {
    return strlen(_footer);
  } else if (_footerPmem) {
    return strlen_P(reinterpret_cast<const char*>(_footerPmem));
  }
  return 0;
}
