#ifndef _sixbuttonui_WidgetModel_h
#define _sixbuttonui_WidgetModel_h


#include "UIElement.h"

// forward declaration
class SixButtonUI;

class WidgetModel {
  public:
    // Force pure virtual
    virtual ~WidgetModel() = 0;

    void setTitle(const char* title, bool allocate = false);
    void setTitle(const __FlashStringHelper* titlePmem);
    // For string literals only (RAM or PROGMEM)
    void setTitleRaw(const char* title, bool pmem);
    bool isTitlePmem() { return _isTitlePmem; };
    const char* getTitle();

    void setInstruction(const char* instruction, bool allocate = false);
    void setInstruction(const __FlashStringHelper* instructionPmem);
    // For string literals only (RAM or PROGMEM)
    void setInstructionRaw(const char* instruction, bool pmem);
    bool isInstructionPmem() { return _isInstructionPmem; };
    const char* getInstruction();

    void setFooter(const char* footer, bool allocate = false);
    void setFooter(const __FlashStringHelper* footerPmem);
    // For string literals only (RAM or PROGMEM)
    void setFooterRaw(const char* footer, bool pmem);
    bool isFooterPmem() { return _isFooterPmem; };
    const char* getFooter();

    // Access SixButtonUI for switching elements
    const SixButtonUI* getController() {
      return _ui;
    };

    // Disable moving and copying
    WidgetModel(WidgetModel&& other) = delete;
    WidgetModel& operator=(WidgetModel&& other) = delete;
    WidgetModel(const WidgetModel&) = delete;
    WidgetModel& operator=(const WidgetModel&) = delete;

  protected:
    WidgetModel() = default;

  private:
    const char* _title = nullptr;
    bool _ownsTitle = false;
    bool _isTitlePmem = false;
    const char* _instruction = nullptr;
    bool _ownsInstruction = false;
    bool _isInstructionPmem = false;
    const char* _footer = nullptr;
    bool _ownsFooter = false;
    bool _isFooterPmem = false;

    SixButtonUI* _ui      = nullptr;

    // Only available to SixButtonUI to pass state to
    // action handler callback functions
    void*        _state   = nullptr;

    friend class SixButtonUI;
};
inline WidgetModel::~WidgetModel() {
  if (_title && _ownsTitle) {
    free(_title);
  }
  if (_instruction && _ownsInstruction) {
    free(_instruction);
  }
  if (_footer && _ownsFooter) {
    free(_footer);
  }
  _title = nullptr;
  _ownsTitle = false;
  _isTitlePmem = false;
  _instruction = nullptr;
  _ownsInstruction = false;
  _isInstructionPmem = false;
  _footer = nullptr;
  _ownsFooter = false;
  _isFooterPmem = false;
}


#endif