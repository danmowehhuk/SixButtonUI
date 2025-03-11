#ifndef _sixbuttonui_UIElement_h
#define _sixbuttonui_UIElement_h


#include <Arduino.h>
#include <stdint.h>

/*
 * Base type for any node in the SixButtonUI navigation tree. Cannot be instantiated
 * directly. Use the convenience functions in the sixbuttonui namespace to instantiate
 * concrete subtypes.
 * 
 * The char arrays for 'title', 'instruction', and 'footer' are expected not to change
 * once initialized, so no memory management is performed for them.
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

    const char* getTitle() const { return _title; };
    bool isTitlePmem() { return _isTitlePmem; };

    const char* getInstruction() const { return _instruction; };
    bool isInstructionPmem() { return _isInstructionPmem; };

    const char* getFooter() const { return _footer; };
    bool isFooterPmem() { return _isFooterPmem; };

    // Make the class pure virtual
    virtual ~UIElement() = 0;

  protected:
    UIElement(Type type): type(type) {};
    void setParent(UIElement* parent);
    UIElement** _children = nullptr;
    uint8_t _numChildren = 0;
    char* _title = nullptr;
    char* _instruction = nullptr;
    char* _footer = nullptr;
    bool _isTitlePmem = false;
    bool _isInstructionPmem = false;
    bool _isFooterPmem = false;

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
  _title = title;
  _isTitlePmem = pmem;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withTitle(const __FlashStringHelper* title) {
  _title = reinterpret_cast<const char*>(title);
  _isTitlePmem = true;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withInstruction(const char* instruction, bool pmem = false) {
  _instruction = instruction;
  _isInstructionPmem = pmem;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withInstruction(const __FlashStringHelper* instruction) {
  _instruction = reinterpret_cast<const char*>(instruction);
  _isInstructionPmem = true;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withFooter(const char* footer, bool pmem = false) {
  _footer = footer;
  _isFooterPmem = pmem;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withFooter(const __FlashStringHelper* footer) {
  _footer = reinterpret_cast<const char*>(footer);
  _isFooterPmem = true;
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