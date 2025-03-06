#ifndef _sixbuttonui_UIElement_h
#define _sixbuttonui_UIElement_h


#include <stdint.h>

/*
 * Base type for any node in the SixButtonUI navigation tree. Cannot be instantiated
 * directly. Use the convenience functions in the sixbuttonui namespace to instantiate
 * concrete subtypes
 */
class UIElement {
  public:  
    enum Type : uint8_t {
      UNDEFINED,
      ROOT,          // MENU button toggles thru top-level UI elements
      SELECTOR,      // UP/DOWN thru options, ENTER selects, MENU goes back, LEFT/RIGHT non-functional
      SUB_MENU,      // Like SELECTOR, but options are child menu elements, LEFT/RIGHT non-functional
      TEXT_INPUT,    // UP/DOWN select character, LEFT/RIGHT move cursor, ENTER and BACK function as expected
      COMBO_BOX,     // Like TEXT_INPUT, but UP/DOWN/LEFT/RIGHT move char by char through a trie of the options
      WIZARD         // Like SELECTOR, but LEFT/RIGHT move thru series of SELECTORS
    };
    const Type type;
    const UIElement* getParent() const;
    const uint8_t getChildCount() const;
    const UIElement* getChild(uint8_t index) const;
    const char* getTitle() const;
    const __FlashStringHelper* getTitle_P() const;
    bool isTitlePmem();
    size_t getTitleLength() const;
    const char* getInstruction() const;
    const __FlashStringHelper* getInstruction_P() const;
    bool isInstructionPmem();
    size_t getInstructionLength() const;
    const char* getFooter() const;
    const __FlashStringHelper* getFooter_P() const;
    bool isFooterPmem();
    size_t getFooterLength() const;

    // Make the class pure virtual
    virtual ~UIElement() = 0;

  protected:
    UIElement(Type type): type(type) {};
    void setParent(UIElement* parent);
    UIElement** _children = nullptr;
    uint8_t _numChildren = 0;
    char* _title = nullptr;
    __FlashStringHelper* _titlePmem = nullptr;
    char* _instruction = nullptr;
    __FlashStringHelper* _instructionPmem = nullptr;
    char* _footer = nullptr;
    __FlashStringHelper* _footerPmem = nullptr;

  private:
    UIElement() = delete;
    UIElement(UIElement &t) = delete;
    UIElement* _parent = nullptr;

    // Grant friend access to all UIElementBase specializations
    template <typename DerivedElement>
    friend class UIElementBase;
};
// Destructor impl required despite being virtual
inline UIElement::~UIElement() {};


/*
 * Additional pieces of the base UIElement class that need to be
 * templated so the API returns the right subtype
 */
template <typename DerivedElement>
class UIElementBase: public UIElement {

  public:
    // Make the class pure virtual
    virtual ~UIElementBase() = 0;
    DerivedElement* withTitle(const char* title, bool pmem = false);
    DerivedElement* withTitle(const __FlashStringHelper* title);
    DerivedElement* withInstruction(const char* instruction, bool pmem = false);
    DerivedElement* withInstruction(const __FlashStringHelper* instruction);
    DerivedElement* withFooter(const char* footer, bool pmem = false);
    DerivedElement* withFooter(const __FlashStringHelper* footer);

 protected:
    UIElementBase(const Type type): UIElement(type) {};

    template <typename... Args>
    DerivedElement* withChildren(UIElement* childElement, Args... moreChildElements);
    DerivedElement* withChildren() { return static_cast<DerivedElement*>(this); };

  private:
    UIElementBase(UIElementBase &t) = delete;
    UIElementBase() = delete;

};
template <typename DerivedElement>
inline UIElementBase<DerivedElement>::~UIElementBase() {}; 


template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withTitle(const char* title, bool pmem = false) {
  if (pmem) {
    _titlePmem = reinterpret_cast<const __FlashStringHelper *>(title);
  } else {
    _title = title;
  }
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withTitle(const __FlashStringHelper* title) {
  _titlePmem = title;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withInstruction(const char* instruction, bool pmem = false) {
  if (pmem) {
    _instructionPmem = reinterpret_cast<const __FlashStringHelper *>(instruction);
  } else {
    _instruction = instruction;
  }
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withInstruction(const __FlashStringHelper* instruction) {
  _instructionPmem = instruction;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withFooter(const char* footer, bool pmem = false) {
  if (pmem) {
    _footerPmem = reinterpret_cast<const __FlashStringHelper *>(footer);
  } else {
    _footer = footer;
  }
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withFooter(const __FlashStringHelper* footer) {
  _footerPmem = footer;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
template <typename... Args>
DerivedElement* UIElementBase<DerivedElement>::withChildren(UIElement* childElement, Args... moreChildElements) {
  UIElement** newChildren = new UIElement*[_numChildren+1];
  for (uint8_t i = 0; i < _numChildren; i++) {
    newChildren[i] = _children[i];
  }
  newChildren[_numChildren] = childElement;
  childElement->setParent(this);
  if (_children) delete[] _children;
  _children = newChildren;
  _numChildren++;
  return withChildren(moreChildElements...);
};


#endif