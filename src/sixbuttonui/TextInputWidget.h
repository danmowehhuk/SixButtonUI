#ifndef _sixbuttonui_TextInputWidget_h
#define _sixbuttonui_TextInputWidget_h


#include "TextInputModel.h"
#include "Widget.h"

class TextInputWidget: public Widget {

  public:
    TextInputWidget(const TextInputElement* config): Widget(config), _config(config) {};
    ~TextInputWidget() {
      if (_model) delete _model;
    };

    void initModel() override {
      _model = new TextInputModel(_config->getInitialValue(), _config->isInitialValuePmem());
      // model is static so only loads once
      _noRefreshModel = true;
    }

    void loadModel(void* state) override {
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::TEXT_INPUT, _model);
      vm.setInteractiveLine(_model->getValue(), false);
      vm.cursorMode = ViewModel::CursorMode::UNDERLINE;
      vm.cursorPosition = _model->getCursorPos();
      return vm;
    };

  protected:
    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      m->getController()->goTo(_config->getParent());
      if (_config->onEnterFunc != 0) {
        state = _config->onEnterFunc(m->getValue(), state);
      }
      return state;
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      char c = m->getChar();
      uint8_t curr = (uint8_t)c;
      uint8_t next = curr;
      switch (curr) {  // always start with ' ', then
        case 32:       // lower-case, upper-case, numbers, then symbols
          next = 97;
          break;
        case 122:
          next = 65;
          break;
        case 90:
          next = 48;
          break;
        case 57:
          next = 33;
          break;
        case 47:
          next = 58;
          break;
        case 64:
          next = 91;
          break;
        case 96:
          next = 123;
          break;
        case 126:
          next = 32;
          break;
        default:
          next++;
          break;
      }
      m->setChar((char)next);
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      char c = m->getChar();
      uint8_t curr = (uint8_t)c;
      uint8_t prev = curr;
      switch (curr) {  // same as onDownPressed, but reverse order
        case 32:
          prev = 126;
          break;
        case 123:
          prev = 96;
          break;
        case 91:
          prev = 64;
          break;
        case 58:
          prev = 47;
          break;
        case 33:
          prev = 57;
          break;
        case 48:
          prev = 90;
          break;
        case 65:
          prev = 122;
          break;
        case 97:
          prev = 32;
          break;
        default:
          prev--;
          break;
      }
      m->setChar((char)prev);
    };

    void onDownLongPressed(uint8_t value, void* widgetModel) override {
      onDownPressed(value, widgetModel);
    };

    void onUpLongPressed(uint8_t value, void* widgetModel) override {
      onUpPressed(value, widgetModel);
    };

    bool onDownLongPressRepeat() override { return true; };
    bool onUpLongPressRepeat() override { return true; };

    void onLeftPressed(uint8_t value, void* widgetModel) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      uint8_t pos = m->getCursorPos();
      if (pos > 0) {
        m->setCursorPos(pos - 1);
      }
    };

    void onRightPressed(uint8_t value, void* widgetModel) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      uint8_t pos = m->getCursorPos();
      if (pos < TextInputModel::MAX_LENGTH - 1) {
        m->setCursorPos(pos + 1);
      }
    };

    void onLeftLongPressed(uint8_t value, void* widgetModel) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      m->initialize(nullptr, false); // clear the value
    };


  private:
    TextInputElement* _config;
    TextInputModel* _model;
    virtual WidgetModel* getModel() override {
      return _model;
    };

};


#endif