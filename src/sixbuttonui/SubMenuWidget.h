#ifndef _sixbuttonui_SubMenuWidget_h
#define _sixbuttonui_SubMenuWidget_h


#include "Widget.h"


int freeMemory();

class SubMenuWidget: public Widget {

  public:
    SubMenuWidget(const SubMenuElement* config): Widget(config), _config(config) {};
    ~SubMenuWidget() {};

    struct SubMenuModel: public Widget::Model {
      char** menuItemNames = nullptr;
      __FlashStringHelper** menuItemNamesPmem = nullptr;
      size_t* menuItemLengths = nullptr;
      UIElement** menuElements = nullptr;
      uint8_t numOptions = 0;
      uint8_t currIndex = 0;

      ~SubMenuModel() {
        if (menuItemNames) {
          delete[] menuItemNames;
          menuItemNames = nullptr;
        }
        if (menuItemNamesPmem) {
          delete[] menuItemNamesPmem;
          menuItemNamesPmem = nullptr;
        }
        if (menuItemLengths) {
          delete[] menuItemLengths;
          menuItemLengths = nullptr;
        }
        if (menuElements) {
          delete[] menuElements;
          menuElements = nullptr;
        }
      };
    };

    Widget::Model* loadModel(void* state) override {
      // submenu model is static so only loads once
      if (modelLoaded) return &_model;

      _configurableModel.title = _config->getTitle();
      _configurableModel.titlePmem = _config->getTitle_P();
      if (_config->modelLoader != 0) {
        _config->modelLoader(&_configurableModel, state);
      }

      uint8_t count = _config->getChildCount();
      _model.menuItemNames = new char*[count];
      _model.menuItemNamesPmem = new __FlashStringHelper*[count];
      _model.menuItemLengths = new size_t[count];
      _model.menuElements = new UIElement*[count];
      _model.numOptions = count;
      for (uint8_t i = 0; i < count; i++) {
        UIElement* child = _config->getChild(i);
        _model.menuItemNames[i] = child->getTitle();
        _model.menuItemNamesPmem[i] = child->getTitle_P();
        _model.menuItemLengths[i] = child->getTitleLength();
        _model.menuElements[i] = child;
      }
      _model.currIndex = _config->lastSelected;
      modelLoaded = true;
      return &_model;
    };

    ViewModel* getViewModel() override {
      size_t titleLength = getLength(_configurableModel.title, _configurableModel.titlePmem);
      char* title = copyMaybePmem(_configurableModel.title, _configurableModel.titlePmem, titleLength);
      size_t instructionLength = _config->getInstructionLength();
      char* instruction = copyMaybePmem(_config->getInstruction(), _config->getInstruction_P(), instructionLength);
      size_t interactiveLength;
      char* interactive;
      if (_model.numOptions > 0) {
        interactiveLength = _model.menuItemLengths[_model.currIndex];
        interactive = copyMaybePmem(_model.menuItemNames[_model.currIndex],
                _model.menuItemNamesPmem[_model.currIndex], interactiveLength);
      } else {
        interactiveLength = 0;
        interactive = "";
      }
      size_t footerLength = _config->getFooterLength();
      char* footer = copyMaybePmem(_config->getFooter(), _config->getFooter_P(), footerLength);

      ViewModel* vm = new ViewModel(title, titleLength, instruction, instructionLength,
              interactive, interactiveLength, footer, footerLength);
      delete[] title;
      delete[] instruction;
      delete[] interactive;
      delete[] footer;
      vm->type = UIElement::Type::SUB_MENU;
      return vm;
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