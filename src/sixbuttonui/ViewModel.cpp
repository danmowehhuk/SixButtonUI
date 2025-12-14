#include "ViewModel.h"

const __FlashStringHelper* ViewModel::getTitleLine_P() {
  return reinterpret_cast<const __FlashStringHelper*>(_titleLine);
}

void ViewModel::setTitleLine(const char* titleLine, bool pmem) {
  _titleLine = titleLine;
  _isTitlePmem = pmem;
}

const __FlashStringHelper* ViewModel::getInstructionLine_P() {
  return reinterpret_cast<const __FlashStringHelper*>(_instructionLine);
}

void ViewModel::setInstructionLine(const char* instructionLine, bool pmem) {
  _instructionLine = instructionLine;
  _isInstructionPmem = pmem;
}

const __FlashStringHelper* ViewModel::getInteractiveLine_P() {
  return reinterpret_cast<const __FlashStringHelper*>(_interactiveLine);
}

void ViewModel::setInteractiveLine(const char* interactiveLine, bool pmem) {
  _interactiveLine = interactiveLine;
  _isInteractivePmem = pmem;
}

const __FlashStringHelper* ViewModel::getFooterLine_P() {
  return reinterpret_cast<const __FlashStringHelper*>(_footerLine);
}

void ViewModel::setFooterLine(const char* footerLine, bool pmem) {
  _footerLine = footerLine;
  _isFooterPmem = pmem;
}

void ViewModel::setUIElementId(uint8_t uiElementId) {
  _uiElementId = uiElementId;
  _hasUIElementId = true;
}

ViewModel::~ViewModel() {
  _titleLine                = nullptr;
  _instructionLine          = nullptr;
  _interactiveLine          = nullptr;
  _footerLine               = nullptr;
}

ViewModel::ViewModel(ViewModel&& other) noexcept:
      _type(other._type),
      _uiElementId(other._uiElementId),
      _hasUIElementId(other._hasUIElementId),
      _titleLine(other._titleLine),
      _isTitlePmem(other._isTitlePmem),
      _instructionLine(other._instructionLine),
      _isInstructionPmem(other._isInstructionPmem),
      _interactiveLine(other._interactiveLine),
      _isInteractivePmem(other._isInteractivePmem),
      _footerLine(other._footerLine),
      _isFooterPmem(other._isFooterPmem),
      hasPrev(other.hasPrev),
      hasNext(other.hasNext),
      isSelected(other.isSelected),
      isSelectable(other.isSelectable),
      cursorMode(other.cursorMode),
      cursorPosition(other.cursorPosition) {
  other._titleLine                = nullptr;
  other._instructionLine          = nullptr;
  other._interactiveLine          = nullptr;
  other._footerLine               = nullptr;
  other._uiElementId              = 0;
  other._hasUIElementId           = false;
}

ViewModel& ViewModel::operator=(ViewModel&& other) noexcept {
  if (this != &other) {
    _type = other._type;
    _uiElementId = other._uiElementId;
    _hasUIElementId = other._hasUIElementId;
    _titleLine = other._titleLine;
    _isTitlePmem = other._isTitlePmem;
    _instructionLine = other._instructionLine;
    _isInstructionPmem = other._isInstructionPmem;
    _interactiveLine = other._interactiveLine;
    _isInteractivePmem = other._isInteractivePmem;
    _footerLine = other._footerLine;
    _isFooterPmem = other._isFooterPmem;
    hasPrev = other.hasPrev;
    hasNext = other.hasNext;
    isSelected = other.isSelected;
    isSelectable = other.isSelectable;
    cursorMode = other.cursorMode;
    cursorPosition = other.cursorPosition;
    other._uiElementId              = 0;
    other._hasUIElementId           = false;
    other._titleLine                = nullptr;
    other._instructionLine          = nullptr;
    other._interactiveLine          = nullptr;
    other._footerLine               = nullptr;
  }
  return *this;
}
