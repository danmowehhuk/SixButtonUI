#ifndef _sixbuttonui_ViewModel_h
#define _sixbuttonui_ViewModel_h


#include "UIElement.h"
#include "WidgetModel.h"

/*
 * The ViewModel is the final output from SixButtonUI that is passed to
 * the rendering function used when instantiating SixButtonUI.
 *
 * Do not pass dynamically allocated strings to the set... methods. This
 * class will not clean them up. It only expects pointers to string 
 * literals or strings owned by a WidgetModel subclass.
 */
class ViewModel {

  public:
    ViewModel(UIElement::Type type, WidgetModel* model): _type(type) {
      setTitleLine(model->getTitle(), model->isTitlePmem());
      setInstructionLine(model->getInstruction(), model->isInstructionPmem());
      setInteractiveLine("", false);
      setFooterLine(model->getFooter(), model->isFooterPmem());
    };
    ViewModel(UIElement::Type type): _type(type) {};
    ~ViewModel();

    // Allow moving
    ViewModel(ViewModel&& other) noexcept;
    ViewModel& operator=(ViewModel&& other) noexcept;
    // But not copying
    ViewModel(const ViewModel&) = delete;
    ViewModel& operator=(const ViewModel&) = delete;

    enum CursorMode : uint8_t {
      NO_CURSOR,
      UNDERLINE,
      SOLID
    };

    const UIElement::Type getType() { return _type; };

    bool isTitlePmem() { return _isTitlePmem; };
    const char* getTitleLine() { return _titleLine; };
    const __FlashStringHelper* getTitleLine_P();
    void setTitleLine(const char* titleLine, bool pmem);
    
    bool isInstructionPmem() { return _isInstructionPmem; };
    const char* getInstructionLine() { return _instructionLine; };
    const __FlashStringHelper* getInstructionLine_P();
    void setInstructionLine(const char* instructionLine, bool pmem);
    
    bool isInteractivePmem() { return _isInteractivePmem; };
    const char* getInteractiveLine() { return _interactiveLine; };
    const __FlashStringHelper* getInteractiveLine_P();
    void setInteractiveLine(const char* interactiveLine, bool pmem);
    
    bool isFooterPmem() { return _isFooterPmem; };
    const char* getFooterLine() { return _footerLine; };
    const __FlashStringHelper* getFooterLine_P();
    void setFooterLine(const char* footerLine, bool pmem);

    CursorMode cursorMode = CursorMode::NO_CURSOR;
    uint8_t cursorPosition = 0;
    bool hasNext = false;
    bool hasPrev = false;
    bool isSelected = false;
    bool isSelectable = false;
    bool isCancelable = false;

  private:
    UIElement::Type _type;

    char* _titleLine = nullptr;
    char* _instructionLine = nullptr;
    char* _interactiveLine = nullptr;
    char* _footerLine = nullptr;
    bool _isTitlePmem = false;
    bool _isInstructionPmem = false;
    bool _isInteractivePmem = false;
    bool _isFooterPmem = false;

    friend class ViewModel;
};


#endif