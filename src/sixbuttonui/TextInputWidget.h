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
      m->setChar(TextInputModel::getNext(m->getChar()));
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      TextInputModel* m = static_cast<TextInputModel*>(widgetModel);
      m->setChar(TextInputModel::getPrev(m->getChar()));
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
      m->setInitialValue(" ", false); // clear the value
    };


  private:
    TextInputElement* _config;
    TextInputModel* _model;
    virtual WidgetModel* getModel() override {
      return _model;
    };

};


#endif