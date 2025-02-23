#ifndef _sixbuttonui_SubMenuWidget_h
#define _sixbuttonui_SubMenuWidget_h


#include "Widget.h"


int freeMemory();

class SubMenuWidget: public Widget {

  public:
    SubMenuWidget(const SubMenuElement* config): Widget(config), _config(config) {};
    ~SubMenuWidget() {};

    struct SubMenuModel: public Widget::Model {
      char* title = nullptr;
      char** menuItemNames = nullptr;
      UIElement** menuElements = nullptr;
      uint8_t numOptions = 0;
      uint8_t currIndex = 0;

      ~SubMenuModel() {
        title = nullptr;
        if (menuItemNames) {
          delete[] menuItemNames;
          menuItemNames = nullptr;
        }
        if (menuElements) {
          delete[] menuElements;
          menuElements = nullptr;
        }
      };
    };

    Widget::Model* loadModel(void* state) override {
      // submenu model is static so only loads once
      if (modelLoaded) {
        return &_model;
      }

      _configurableModel.title = _config->getTitle();
      if (_config->modelLoader != 0) {
        _config->modelLoader(&_configurableModel, state);
      }

      _model.title = _configurableModel.title;
      uint8_t count = _config->getChildCount();
      _model.menuItemNames = new char*[count];
      _model.menuElements = new UIElement*[count];
      _model.numOptions = count;
      for (uint8_t i = 0; i < count; i++) {
        UIElement* child = _config->getChild(i);
        _model.menuItemNames[i] = child->getTitle();
        _model.menuElements[i] = child;
      }
      _model.currIndex = _config->lastSelected;
      modelLoaded = true;
      return &_model;
    };

    void updateViewModel(ViewModel* vm) override {
      vm->type = UIElement::Type::SUB_MENU;
      strncpy(vm->titleLine, _model.title, ViewModel::MAX_TITLE_LENGTH);
      if (_model.numOptions > 0) {
        strncpy(vm->interactiveLine, _model.menuItemNames[_model.currIndex], ViewModel::MAX_INTER_LENGTH);
      } else {
        strncpy(vm->interactiveLine, "", ViewModel::MAX_INTER_LENGTH);
      }
    };

  protected:
    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SubMenuModel* m = static_cast<SubMenuModel*>(widgetModel);
      if (m->numOptions == 0) return state; // nothing to select
      _config->lastSelected = m->currIndex;
      m->ui->goTo(m->menuElements[m->currIndex]);
      return state;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      SubMenuModel* m = static_cast<SubMenuModel*>(widgetModel);
      if (m->currIndex > 0) m->currIndex--;
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      SubMenuModel* m = static_cast<SubMenuModel*>(widgetModel);
      if (m->currIndex < m->numOptions - 1) m->currIndex++;
    };

  private:
    SubMenuElement* _config;
    SubMenuElement::Model _configurableModel;
    SubMenuModel _model;
    bool modelLoaded = false;

};


#endif