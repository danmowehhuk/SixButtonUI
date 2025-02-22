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
    char* getTitle();
    char* getInstruction();

    // Make the class pure virtual
    virtual ~UIElement() = 0;

  protected:
    UIElement(Type type): type(type) {};
    void setParent(UIElement* parent);
    UIElement** _children = nullptr;
    uint8_t _numChildren = 0;
    char* _title = nullptr;
    bool _pmemTitle = false;
    char* _instruction = nullptr;
    bool _pmemInstruction = false;

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
    DerivedElement* withInstruction(const char* instruction, bool pmem = false);

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
  _pmemTitle = pmem;
  return static_cast<DerivedElement*>(this);
};

template <typename DerivedElement>
DerivedElement* UIElementBase<DerivedElement>::withInstruction(const char* instruction, bool pmem = false) {
  _instruction = instruction;
  _pmemInstruction = pmem;
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