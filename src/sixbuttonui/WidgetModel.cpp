#include "WidgetModel.h"

void WidgetModel::setTitle(const char* title, bool allocate = false) { 
  if (_title && _ownsTitle) {
    free(_title);
  }
  _title = allocate ? strdup(title) : title;
  _ownsTitle = allocate;
  _isTitlePmem = false;
}

void WidgetModel::setTitle(const __FlashStringHelper* titlePmem) { 
  if (_title && _ownsTitle) {
    free(_title);
  }
  _title = reinterpret_cast<const char*>(titlePmem);
  _ownsTitle = false;
  _isTitlePmem = true;
}

const char* WidgetModel::getTitle() {
  return _title;
}

void WidgetModel::setTitleRaw(const char* title, bool pmem) {
  _title = reinterpret_cast<const char*>(title);
  _isTitlePmem = pmem;
  _ownsTitle = false;
}

void WidgetModel::setInstruction(const char* instruction, bool allocate = false) { 
  if (_instruction && _ownsInstruction) {
    free(_instruction);
  }
  _instruction = allocate ? strdup(instruction) : instruction;
  _ownsInstruction = allocate;
  _isInstructionPmem = false;
}

void WidgetModel::setInstruction(const __FlashStringHelper* instructionPmem) { 
  if (_instruction && _ownsInstruction) {
    free(_instruction);
  }
  _instruction = reinterpret_cast<const char*>(instructionPmem);
  _ownsInstruction = false;
  _isInstructionPmem = true;
}

const char* WidgetModel::getInstruction() {
  return _instruction;
}

void WidgetModel::setInstructionRaw(const char* instruction, bool pmem) {
  _instruction = reinterpret_cast<const char*>(instruction);
  _isInstructionPmem = pmem;
  _ownsInstruction = false;
}

void WidgetModel::setFooter(const char* footer, bool allocate = false) { 
  if (_footer && _ownsFooter) {
    free(_footer);
  }
  _footer = allocate ? strdup(footer) : footer;
  _ownsFooter = allocate;
  _isFooterPmem = false;
}

void WidgetModel::setFooter(const __FlashStringHelper* footerPmem) { 
  if (_footer && _ownsFooter) {
    free(_footer);
  }
  _footer = reinterpret_cast<const char*>(footerPmem);
  _ownsFooter = false;
  _isFooterPmem = true;
}

const char* WidgetModel::getFooter() {
  return _footer;
}

void WidgetModel::setFooterRaw(const char* footer, bool pmem) {
  _footer = reinterpret_cast<const char*>(footer);
  _isFooterPmem = pmem;
  _ownsFooter = false;
}
