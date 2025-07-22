#ifndef _sixbuttonui_WizardWidget_h
#define _sixbuttonui_WizardWidget_h


#include "WizardModel.h"
#include "Widget.h"

class WizardWidget: public Widget {

  public:
    WizardWidget(const WizardElement* config): Widget(config), _config(config) {};
    ~WizardWidget() {
      if (_model) delete _model;
    };

  protected:
    void initModel() override {
      _model = new WizardModel();
      // model is static so only loads once
      _noRefreshModel = true;
    };

    void loadModel(void* state) override {
      // if (_config->modelLoader != 0) {
      //   _config->modelLoader(_model, state);
      // }
    };

  private:
    WizardElement* _config;
    WizardModel* _model;
    virtual WidgetModel* getModel() override {
      return _model;
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::WIZARD, _model);

      return vm;
    };

  };

#endif
