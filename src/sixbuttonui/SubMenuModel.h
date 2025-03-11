#ifndef _sixbuttonui_SubMenuModel_h
#define _sixbuttonui_SubMenuModel_h


#include "WidgetModel.h"

class SubMenuModel : public WidgetModel {
  public:
    SubMenuModel(uint8_t numOptions);
    ~SubMenuModel() override { clear(); };
    uint8_t getNumOptions() const { return _numOptions; };
    uint8_t getCurrIndex() const { return _currIndex; };

    // Menu item data for the current index
    bool isMenuItemNamePmem();
    const char* getMenuItemName();
    UIElement* getMenuItemValue();

    // Disable moving and copying
    SubMenuModel(SubMenuModel&& other) = delete;
    SubMenuModel& operator=(SubMenuModel&& other) = delete;
    SubMenuModel(const SubMenuModel&) = delete;
    SubMenuModel& operator=(const SubMenuModel&) = delete;

  private:
    const char** _menuItemNames = nullptr;
    bool* _menuItemNamePmem = nullptr;
    UIElement** _menuElements = nullptr;
    uint8_t _numOptions = 0;
    uint8_t _currIndex = 0;

    void setMenuItem(uint8_t index, const char* name, bool pmem, UIElement* value);
    void clear();

    friend class SubMenuWidget;
};


#endif
