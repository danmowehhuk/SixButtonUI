#include "SubMenuModel.h"

SubMenuModel::SubMenuModel(uint8_t numOptions): _numOptions(numOptions) {
  if (numOptions > 0) {
    _menuItemNames = new const char*[_numOptions]();
    _menuItemNamePmem = new bool[_numOptions]();
    _menuElements = new UIElement*[_numOptions]();
  }
}

void SubMenuModel::setMenuItem(uint8_t index, const char* name, bool pmem, UIElement* element) {
  if (index < _numOptions) {
    _menuItemNames[index] = name;
    _menuItemNamePmem[index] = pmem;
    _menuElements[index] = element;
  }
}

bool SubMenuModel::isMenuItemNamePmem() {
  return _menuItemNamePmem[_currIndex];
}

const char* SubMenuModel::getMenuItemName() {
  return _menuItemNames[_currIndex];
}

UIElement* SubMenuModel::getMenuItemValue() {
  return _menuElements[_currIndex];
}

void SubMenuModel::clear() {
  delete[] _menuItemNames;
  delete[] _menuItemNamePmem;
  delete[] _menuElements;

  _menuItemNames = nullptr;
  _menuItemNamePmem = nullptr;
  _menuElements = nullptr;
  _numOptions = 0;
  _currIndex = 0;
}

