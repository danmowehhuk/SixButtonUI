#ifndef _sixbuttonui_Widget_h
#define _sixbuttonui_Widget_h


#include "UIElement.h"

// forward declaration
class SixButtonUI;

enum CursorMode : uint8_t {
  NO_CURSOR,
  UNDERLINE,
  SOLID
};

struct ViewModel {
    static constexpr size_t MAX_TITLE_LENGTH = 32;
    static constexpr size_t MAX_INSTR_LENGTH = 64;
    static constexpr size_t MAX_INTER_LENGTH = 32;
    static constexpr size_t MAX_NAV_LENGTH = 32;

    UIElement::Type type = UIElement::Type::UNDEFINED;
    char titleLine[MAX_TITLE_LENGTH] = {0};
    char instructionLine[MAX_INSTR_LENGTH] = {0};
    char interactiveLine[MAX_INTER_LENGTH] = {0};
    char navLine[MAX_NAV_LENGTH] = {0};

    CursorMode cursorMode = CursorMode::NO_CURSOR;
    uint8_t cursorPosition = 0;
    bool hasNext = false;
    bool hasPrev = false;
    bool isSelected = false;

    // Default Constructor
    ViewModel() = default;

    // Deep-Copy Constructor (without heap allocation)
    ViewModel(const ViewModel& orig) {
        copyFrom(orig);
    }

    // Copy Assignment Operator
    ViewModel& operator=(const ViewModel& orig) {
        if (this != &orig) {  // Avoid self-assignment
            copyFrom(orig);
        }
        return *this;
    }

    // Move Constructor
    ViewModel(ViewModel&& other) noexcept {
        copyFrom(other);
    }

    // Move Assignment Operator
    ViewModel& operator=(ViewModel&& other) noexcept {
        if (this != &other) {  // Avoid self-move
            copyFrom(other);
        }
        return *this;
    }

private:
    void copyFrom(const ViewModel& orig) {
        type = orig.type;
        strncpy(titleLine, orig.titleLine, MAX_TITLE_LENGTH);
        strncpy(instructionLine, orig.instructionLine, MAX_INSTR_LENGTH);
        strncpy(interactiveLine, orig.interactiveLine, MAX_INTER_LENGTH);
        strncpy(navLine, orig.navLine, MAX_NAV_LENGTH);
        cursorMode = orig.cursorMode;
        cursorPosition = orig.cursorPosition;
        hasNext = orig.hasNext;
        hasPrev = orig.hasPrev;
        isSelected = orig.isSelected;
    }
};

class Widget {

  public:
    virtual ~Widget() = 0;
    struct Model {
      SixButtonUI* ui = nullptr;
      UIElement* element = nullptr;
      void* state = nullptr;
      ~Model() {
        ui = nullptr;
        element = nullptr;
        state = nullptr;
      };
    };

  protected:
    Widget(UIElement* wConf): wConf(wConf) {};
    const UIElement* wConf;

    virtual void updateViewModel(ViewModel* vm) = 0;
    virtual Model* loadModel(void* state) = 0;
    void initWidgetModel(void* state) {
      _widgetModel = loadModel(state);
    };
    Model* widgetModel() {
      return _widgetModel;
    };

    /*
     * onEnter returns either the same state object passed in or
     * a new one to be passed to future onEnter calls
     */
    virtual void* onEnter(uint8_t value, void* widgetModel, void* state) {};

    virtual void onUpPressed(uint8_t value, void* widgetModel) {};
    virtual void onDownPressed(uint8_t value, void* widgetModel) {};
    virtual void onLeftPressed(uint8_t value, void* widgetModel) {};
    virtual void onRightPressed(uint8_t value, void* widgetModel) {};

    friend class SixButtonUI;

  private:
    Widget(Widget &t) = delete;
    Model* _widgetModel = nullptr;

};
inline Widget::~Widget() {};


#endif
