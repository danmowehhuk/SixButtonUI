#ifndef _sixbuttonui_SubMenuWidget_h
#define _sixbuttonui_SubMenuWidget_h


#include "SubMenuModel.h"
#include "Widget.h"

class SubMenuWidget: public Widget {

  public:
    SubMenuWidget(const SubMenuElement* config): Widget(config), _config(config) {};
    ~SubMenuWidget() {
      if (_model) delete _model;
    };

  protected:
    void initModel() override {
      uint8_t count = _config->getChildCount();
      _model = new SubMenuModel(count);
      // submenu model is static so only loads once
      _noRefreshModel = true;
    }

    void loadModel(void* state) override {
      _model->_currIndex = _config->lastSelected;

      // Load the non-customizable fields from the element configuration
      uint8_t count = _config->getChildCount();
      for (uint8_t i = 0; i < count; i++) {
        UIElement* child = _config->getChild(i);
        _model->setMenuItem(i, child->getTitle(), child->isTitlePmem(), child);
      }

      // Call custom model loader function
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::SUB_MENU, _model);
      if (_model->getNumOptions() > 0) {
        vm.setInteractiveLine(_model->getMenuItemName(), _model->isMenuItemNamePmem());
      }
      return vm;
    };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SubMenuModel* m = static_cast<SubMenuModel*>(widgetModel);
      if (m->getNumOptions() == 0) return state; // nothing to select
      uint8_t currIdx = m->getCurrIndex();
      _config->lastSelected = currIdx;
      m->getController()->goTo(m->getMenuItemValue());
      return state;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      SubMenuModel* m = static_cast<SubMenuModel*>(widgetModel);
      if (m->_currIndex > 0) m->_currIndex--;
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      SubMenuModel* m = static_cast<SubMenuModel*>(widgetModel);
      if (m->_currIndex < m->_numOptions - 1) m->_currIndex++;
    };

  private:
    SubMenuElement* _config;
    SubMenuModel* _model = nullptr;
    virtual WidgetModel* getModel() override {
      return _model;
    };

};


#endif