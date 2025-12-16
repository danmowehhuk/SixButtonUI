#include "WidgetModel.h"

void WidgetModel::setTitle(const char* title, bool allocate) { 
  setTitleRaw(title, false, allocate);
}

void WidgetModel::setTitle(const __FlashStringHelper* titlePmem) { 
  setTitleRaw(reinterpret_cast<const char*>(titlePmem), true, false);
}

void WidgetModel::setTitleRaw(const char* title, bool isPmem, bool allocate) {
  if (_title && _ownsTitle) {
    free(_title);
  }
  if (!title || isPmem || !allocate) {
    _title = title;
    _ownsTitle = false;
  } else {
    _title = strdup(title);
    _ownsTitle = true;
  }
  _isTitlePmem = isPmem;
}

const char* WidgetModel::getTitle() {
  return _title;
}

void WidgetModel::setInstruction(const char* instruction, bool allocate) { 
  setInstructionRaw(instruction, false, allocate);
}

void WidgetModel::setInstruction(const __FlashStringHelper* instructionPmem) { 
  setInstructionRaw(reinterpret_cast<const char*>(instructionPmem), true, false);
}

void WidgetModel::setInstructionRaw(const char* instruction, bool isPmem, bool allocate) {
  if (_instruction && _ownsInstruction) {
    free(_instruction);
  }
  if (!instruction || isPmem || !allocate) {
    _instruction = instruction;
    _ownsInstruction = false;
  } else {
    _instruction = strdup(instruction);
    _ownsInstruction = true;
  }
  _isInstructionPmem = isPmem;
}

const char* WidgetModel::getInstruction() {
  return _instruction;
}

void WidgetModel::setFooter(const char* footer, bool allocate) { 
  setFooterRaw(footer, false, allocate);
}

void WidgetModel::setFooter(const __FlashStringHelper* footerPmem) { 
  setFooterRaw(reinterpret_cast<const char*>(footerPmem), true, false);
}

void WidgetModel::setFooterRaw(const char* footer, bool isPmem, bool allocate) {
  if (_footer && _ownsFooter) {
    free(_footer);
  }
  if (!footer || isPmem || !allocate) {
    _footer = footer;
    _ownsFooter = false;
  } else {
    _footer = strdup(footer);
    _ownsFooter = true;
  }
  _isFooterPmem = isPmem;
}

const char* WidgetModel::getFooter() {
  return _footer;
}
