#ifndef _sixbuttonui_WizardWidget_h
#define _sixbuttonui_WizardWidget_h

#include "SelectorElement.h"
#include "SelectorModel.h"
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
      _model = new WizardModel(_config->getChildCount());
      
      // the WizardModel is static, but the SelectorModel must be loaded
      // whenever the step changes
      _noRefreshModel = false;
    };

    void loadModel(void* state) override {
      // Load the wizard model only on first load
      if (!_wizardModelLoaded && _config->modelLoader != 0) {
        _config->modelLoader(_model, state);
        _wizardModelLoaded = true;
      }

      // Maybe update the selector model
      if (!_doModelReload) return;
      SelectorElement* selectorElement = (SelectorElement*)_config->getChild(_model->_currStep);
      _model->loadSelectorModel(selectorElement, state);
      _doModelReload = false;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      WizardModel* m = static_cast<WizardModel*>(widgetModel);
      m->prevSelection();
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      WizardModel* m = static_cast<WizardModel*>(widgetModel);
      m->nextSelection();
    };

    void onLeftPressed(uint8_t value, void* widgetModel) override {
      WizardModel* m = static_cast<WizardModel*>(widgetModel);
      _doModelReload = m->prevStep();
    };

    void onRightPressed(uint8_t value, void* widgetModel) override {
      WizardModel* m = static_cast<WizardModel*>(widgetModel);
      _doModelReload = m->nextStep();
    };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      WizardModel* m = static_cast<WizardModel*>(widgetModel);
      if (m->_numSteps > 0) {

        // Do this before calling onEnterFunc so it can be overridden
        m->getController()->setNextDefault();

        if (_config->onEnterFunc != 0) {
          m->captureStepSelection();
          state = _config->onEnterFunc(m->_selectionValues, m->_numSteps, state);
        }
      }
      return state;
    };


  private:
    WizardElement* _config;
    WizardModel* _model = nullptr;
    bool _doModelReload = true;
    bool _wizardModelLoaded = false;
    
    virtual WidgetModel* getModel() override {
      return _model;
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::WIZARD, _model);
      if (_model->_stepTitle) {
        vm.setTitleLine(_model->_stepTitle, _model->_stepTitlePmem);
      }
      if (_model->_selectorModel && _model->_selectorModel->getNumOptions() > 0) {
        vm.setInteractiveLine(_model->_selectorModel->getOptionName(), _model->_selectorModel->isOptionNamePmem());
        vm.isSelected = _model->_selectorModel->isCurrValueSelected();
      }
      if (_model->_currStep > 0) {
        vm.hasPrev = true;
      }
      if (_model->_currStep < _model->_numSteps - 1) {
        vm.hasNext = true;
      }

      return vm;
    };

};

#endif
