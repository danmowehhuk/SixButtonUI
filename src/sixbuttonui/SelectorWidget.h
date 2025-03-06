#ifndef _sixbuttonui_SelectorWidget_h
#define _sixbuttonui_SelectorWidget_h


#include "Widget.h"

class SelectorWidget: public Widget {

  public:
    SelectorWidget(const SelectorElement* config): Widget(config), _config(config) {};
    ~SelectorWidget() {};

    struct SelectorModel: public Widget::Model, public SelectorElement::Model {
      ~SelectorModel() {};
    };

    Widget::Model* loadModel(void* state) override {
      // selector model is static so only loads once
      if (modelLoaded) return &_model;

      if (_config->modelLoader != 0) {
        _config->modelLoader(&_model, state);
      }
      if (_model.currValue) {
        for (uint8_t i = 0; i < _model.numOptions; i++) {
          if ((_model.optionValues[i] && strcmp(_model.currValue, _model.optionValues[i]) == 0) ||
              (_model.optionValuesPmem[i] 
                && strcmp_P(_model.currValue, reinterpret_cast<const char*>(_model.optionValuesPmem[i])) == 0)) {
            _model.currIndex = i;
            break;
          }
        }
      } else {
        _model.currIndex = _config->lastSelected;
      }
      modelLoaded = true;
      return &_model;
    };

    ViewModel* getViewModel() override {
      // size_t titleLength = _config->getTitleLength();
      // char* title = copyMaybePmem(_config->getTitle(), _config->getTitle_P(), titleLength);
      // size_t instructionLength = _config->getInstructionLength();
      // char* instruction = copyMaybePmem(_config->getInstruction(), _config->getInstruction_P(), instructionLength);
      // size_t interactiveLength;
      // char* interactive;
      // if (_model.numOptions > 0) {
      //   interactiveLength = _model.optionNameLengths[_model.currIndex];
      //   interactive = copyMaybePmem(_model.optionNames[_model.currIndex],
      //           _model.optionNamesPmem[_model.currIndex], interactiveLength);
      // } else {
        // interactiveLength = 0;
        // interactive = "";
      // }
      // size_t footerLength = _config->getFooterLength();
      // char* footer = copyMaybePmem(_config->getFooter(), _config->getFooter_P(), footerLength);

      ViewModel* vm = new ViewModel("", 0, "", 0, "", 0, "", 0);
      // delete[] title;
      // delete[] instruction;
      // delete[] interactive;
      // delete[] footer;
      vm->type = UIElement::Type::SELECTOR;
      return vm;

      // vm->type = UIElement::Type::SELECTOR;
      // strncpy(vm->titleLine, _model.title, ViewModel::MAX_TITLE_LENGTH);
      // strncpy(vm->instructionLine, _model.instruction, ViewModel::MAX_INSTR_LENGTH);
      // if (_model.numOptions > 0) {
      //   strncpy(vm->interactiveLine, _model.optionNames[_model.currIndex], ViewModel::MAX_INTER_LENGTH);
      // } else {
      //   strncpy(vm->interactiveLine, "", ViewModel::MAX_INTER_LENGTH);
      // }
      // if (strcmp(_model.optionValues[_model.currIndex], _model.currValue) == 0) {
      //   vm->isSelected = true;
      // }
    };

  protected:
    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->numOptions == 0) return; // nothing to select
      _config->lastSelected = m->currIndex;
      if (_config->onEnterFunc != 0) {
        char* selectionName = m->optionNames[m->currIndex];
        char* selectionValue = m->optionValues[m->currIndex];
        state = _config->onEnterFunc(selectionName, selectionValue, state);
      }
      return state;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->currIndex > 0) m->currIndex--;
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->currIndex < m->numOptions - 1) m->currIndex++;
    };

  private:
    SelectorElement* _config;
    SelectorModel _model;
    bool modelLoaded = false;

};


#endif
