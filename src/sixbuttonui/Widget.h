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
  ViewModel(const char* titleLine, const size_t titleLength,
            const char* instructionLine, const size_t instructionLength,
            const char* interactiveLine, const size_t interactiveLineLength,
            const char* footerLine, const size_t footerLength):
        titleLength(titleLength), instructionLength(instructionLength),
        interactiveLineLength(interactiveLineLength), footerLength(footerLength) {

    this->titleLine = titleLine ? strdup(titleLine) : nullptr;
    this->instructionLine = instructionLine ? strdup(instructionLine) : nullptr;
    this->interactiveLine = interactiveLine ? strdup(interactiveLine) : nullptr;
    this->footerLine = footerLine ? strdup(footerLine) : nullptr;
  };

  UIElement::Type type = UIElement::Type::UNDEFINED;
  char* titleLine = nullptr;
  size_t titleLength;
  char* instructionLine = nullptr;
  size_t instructionLength;
  char* interactiveLine = nullptr;
  size_t interactiveLineLength;
  char* footerLine = nullptr;
  size_t footerLength;
  CursorMode cursorMode = CursorMode::NO_CURSOR;
  uint8_t cursorPosition = 0;
  bool hasNext = false;
  bool hasPrev = false;
  bool isSelected = false;

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

  ~ViewModel() {
    if (titleLine) free(titleLine);
    titleLine = nullptr;
    if (instructionLine) free(instructionLine);
    instructionLine = nullptr;
    if (interactiveLine) free(interactiveLine);
    interactiveLine = nullptr;
    if (footerLine) free(footerLine);
    footerLine = nullptr;
  }

private:
  void copyFrom(const ViewModel& orig) {
    delete[] titleLine;
    delete[] instructionLine;
    delete[] interactiveLine;
    delete[] footerLine;

    type = orig.type;
    cursorMode = orig.cursorMode;
    cursorPosition = orig.cursorPosition;
    hasNext = orig.hasNext;
    hasPrev = orig.hasPrev;
    isSelected = orig.isSelected;

    titleLength = orig.titleLength;
    titleLine = (orig.titleLine && orig.titleLength > 0) ? new char[orig.titleLength + 1] : nullptr;
    if (titleLine) strncpy(titleLine, orig.titleLine, titleLength + 1);

    instructionLength = orig.instructionLength;
    instructionLine = (orig.instructionLine && instructionLength > 0) ? new char[instructionLength + 1] : nullptr;
    if (instructionLine) strncpy(instructionLine, orig.instructionLine, instructionLength + 1);

    interactiveLineLength = orig.interactiveLineLength;
    interactiveLine = (orig.interactiveLine && interactiveLineLength > 0) ? new char[interactiveLineLength + 1] : nullptr;
    if (interactiveLine) strncpy(interactiveLine, orig.interactiveLine, interactiveLineLength + 1);

    footerLength = orig.footerLength;
    footerLine = (orig.footerLine && footerLength > 0) ? new char[footerLength + 1] : nullptr;
    if (footerLine) strncpy(footerLine, orig.footerLine, footerLength + 1);
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

    virtual ViewModel* getViewModel() = 0;
    virtual Model* loadModel(void* state) = 0;
    void initWidgetModel(void* state) {
      _widgetModel = loadModel(state);
    };
    Model* widgetModel() {
      return _widgetModel;
    };
    size_t getLength(const char* str, const __FlashStringHelper* str_P) {
      if (str) {
        return strlen(str);
      } else if (str_P) {
        return strlen_P(reinterpret_cast<const char*>(str_P));
      } else {
        return 0;
      }
    };
    char* copyMaybePmem(const char* str, const __FlashStringHelper* str_P, size_t len) {
      char* out = new char[len+1];
      if (!out) return nullptr;
      memset(out, 0, len+1);
      if (str) {
        strncpy(out, str, len);
      } else if (str_P) {
        strncpy_P(out, reinterpret_cast<const char*>(str_P), len);
      }
      out[len] = '\0';
      return out;
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
