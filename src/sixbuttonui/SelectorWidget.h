#ifndef _sixbuttonui_SelectorWidget_h
#define _sixbuttonui_SelectorWidget_h


#include <Arduino.h>
#include "SelectorModel.h"
#include "Widget.h"

class SelectorWidget: public Widget {

  public:
    SelectorWidget(const SelectorElement* config): Widget(config), _config(config) {};
    ~SelectorWidget() override {
      if (_model) delete _model;
    };

  protected:
    SelectorModel* _model = nullptr;

    virtual void initModel() override {
      _model = new SelectorModel();
      // model is static so only loads once
      _noRefreshModel = true;
    }

    void loadModel(void* state) override {
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }
      _model->selectOptionWithValue(_model->_currValue);
    };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->_numOptions > 0) {
        char* selectionValue = m->_optionValues[m->_currIndex];
        if (selectionValue != nullptr) {

          // Do this before calling onEnterFunc so it can be overridden
          m->getController()->goToDefault();

          if (_config->onEnterFunc != 0) {
            char* selectionName = m->_optionNames[m->_currIndex];
            bool isNamePmem = m->_isOptionNamePmem[m->_currIndex];
            bool isValuePmem = m->_isOptionValuePmem[m->_currIndex];
            char* searchPrefix = m->getSearchPrefix();
            if (searchPrefix && strlen(searchPrefix) > 0) {
              // handle the ComboBox case - concatenate searchPrefix and selectionName in a static buffer
              static char buffer[64]; // Enough for prefix + value + null terminator
              uint8_t pos = 0;
              // Copy searchPrefix (in RAM)
              const char* src = searchPrefix;
              while (pos < 63 && *src != '\0') {
                buffer[pos++] = *src++;
              }
              // Append selectionValue (may be in PROGMEM)
              if (isValuePmem) {
                src = selectionName;
                while (pos < 63) {
                  char c = pgm_read_byte(src);
                  if (c == '\0') break;
                  buffer[pos++] = c;
                  src++;
                }
              } else {
                src = selectionName;
                while (pos < 63 && *src != '\0') {
                  buffer[pos++] = *src++;
                }
              }
              buffer[pos] = '\0';
              selectionName = buffer;
              isValuePmem = false; // buffer is in RAM, not PROGMEM
            }
            state = _config->onEnterFunc(selectionName, isNamePmem, selectionValue, isValuePmem, state);
          }
        }
      }
      return state;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->prev();
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->next();
    };


  private:
    SelectorElement* _config;

    virtual WidgetModel* getModel() override {
      return _model;
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::SELECTOR, _model);
      if (_model->getNumOptions() > 0) {
        vm.setInteractiveLine(_model->getOptionName(), _model->isOptionNamePmem());
        vm.isSelected = _model->isCurrValueSelected();
      }
      return vm;
    };

};


#endif
