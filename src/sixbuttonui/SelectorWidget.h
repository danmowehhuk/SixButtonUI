#ifndef _sixbuttonui_SelectorWidget_h
#define _sixbuttonui_SelectorWidget_h


#include "SelectorModel.h"
#include "Widget.h"

class SelectorWidget: public Widget {

  public:
    SelectorWidget(const SelectorElement* config): Widget(config), _config(config) {};
    ~SelectorWidget() override {
      if (_model) delete _model;
    };

    void initModel() override {
      _model = new SelectorModel();
      // model is static so only loads once
      _noRefreshModel = true;
    }

    void loadModel(void* state) override {
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }
      if (_model->_currValue) {
        for (uint8_t i = 0; i < _model->_numOptions; i++) {
          if ((_model->_optionValues[i] && strcmp(_model->_currValue, _model->_optionValues[i]) == 0) ||
              (_model->_isOptionValuePmem[i] 
                && strcmp_P(_model->_currValue, _model->_optionValues[i]) == 0)) {
            _model->_currIndex = i;
            break;
          }
        }
      }
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::SELECTOR, _model);
      if (_model->getNumOptions() > 0) {
        vm.setInteractiveLine(_model->getOptionName(), _model->isOptionNamePmem());
      }
      return vm;
    };

  protected:
    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->getController()->goTo(_config->getParent());
      if (m->_numOptions > 0) {
        if (_config->onEnterFunc != 0) {
          char* selectionName = m->_optionNames[m->_currIndex];
          bool isNamePmem = m->_isOptionNamePmem[m->_currIndex];
          char* selectionValue = m->_optionValues[m->_currIndex];
          bool isValuePmem = m->_isOptionValuePmem[m->_currIndex];
          state = _config->onEnterFunc(selectionName, isNamePmem, selectionValue, isValuePmem, state);
        }
      }
      return state;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->_currIndex > 0) m->_currIndex--;
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->_currIndex < m->_numOptions - 1) m->_currIndex++;
    };

  private:
    SelectorElement* _config;
    SelectorModel* _model = nullptr;
    virtual WidgetModel* getModel() override {
      return _model;
    };

};


#endif
