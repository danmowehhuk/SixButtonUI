#ifndef _sixbuttonui_WizardModel_h
#define _sixbuttonui_WizardModel_h


#include "Strings.h"
#include "WidgetModel.h"
#include "SelectorModel.h"
#include "SelectorElement.h"

using namespace SixButtonUIStrings;

class WizardModel : public WidgetModel {

  public:
    WizardModel(const uint8_t numSteps): _numSteps(numSteps), _hasNext(numSteps > 1), _hasPrev(false) {
      _selectionNames = new char*[numSteps]();
      _selectionValues = new char*[numSteps]();
    };

    ~WizardModel() override { 
      if (_selectorModel) delete _selectorModel;
      clear();
    };

    void setStepInitialValue(const uint8_t step, const char* value);
    void setStepInitialValue(const uint8_t step, const __FlashStringHelper* value);
    void setStepInitialValueRaw(const uint8_t step, const char* value, bool isPmem);
    void setStepTitle(const char* title, bool allocate = true);
    void setStepTitle(const __FlashStringHelper* title);
    void setStepTitleRaw(const char* title, bool isPmem, bool allocate);

    // Set the initial step to start the wizard (default is 0)
    void setInitialStep(const uint8_t step);

    // Disable moving to the previous or next step. This is normally handled internally,
    // but is useful for dynamic wizards where the number of steps is not known up front.
    void prevDisabled(bool disabled) { _hasPrev = !disabled; };
    void nextDisabled(bool disabled) { _hasNext = !disabled; };

    // Disable moving and copying
    WizardModel(WizardModel&& other) = delete;
    WizardModel& operator=(WizardModel&& other) = delete;
    WizardModel(const WizardModel&) = delete;
    WizardModel& operator=(const WizardModel&) = delete;

  private:
    SelectorModel* _selectorModel = nullptr;
    const uint8_t _numSteps;
    uint8_t _currStep = 0;
    bool _hasPrev;
    bool _hasNext;
    char* _stepTitle = nullptr;
    bool _stepTitlePmem = false;
    bool _ownsStepTitle = false;
    char** _selectionNames = nullptr;
    char** _selectionValues = nullptr;

    bool nextStep() {
      if (!_hasNext) return false;
      captureStepSelection();
      _currStep++;
      _hasNext = _currStep < _numSteps - 1;
      _hasPrev = _currStep > 0;
      return true;
    }

    bool prevStep() {
      if (!_hasPrev) return false;
      captureStepSelection();
      _currStep--;
      _hasPrev = _currStep > 0;
      _hasNext = _currStep < _numSteps - 1;
      return true;
    }

    void nextSelection() {
      if (!_selectorModel) return;
      _selectorModel->next();
    }

    void prevSelection() {
      if (!_selectorModel) return;
      _selectorModel->prev();
    }

    void loadSelectorModel(SelectorElement* selectorElement, void* state) {
      if (_selectorModel) delete _selectorModel;
      _selectorModel = new SelectorModel();
      if (selectorElement) {
        if (selectorElement->getTitle()) {
          setStepTitleRaw(selectorElement->getTitle(), selectorElement->isTitlePmem(), false);
        }
        if (selectorElement->modelLoader != 0) {
          selectorElement->modelLoader(_selectorModel, state);
          if (_selectionValues[_currStep]) {
            _selectorModel->selectOptionWithValue(_selectionValues[_currStep]);
          }
        }
      }
    };
    
    void captureStepSelection() {
      if (_selectionNames[_currStep]) free(_selectionNames[_currStep]);
      if (_selectionValues[_currStep]) free(_selectionValues[_currStep]);
      _selectionNames[_currStep] = _selectorModel->isOptionNamePmem() ? strdup_P(_selectorModel->getOptionName()) : strdup(_selectorModel->getOptionName());
      _selectionValues[_currStep] = _selectorModel->isOptionValuePmem() ? strdup_P(_selectorModel->getOptionValue()) : strdup(_selectorModel->getOptionValue());
    };

    void clear() {
      for (uint8_t i = 0; i < _numSteps; i++) {
        if (_selectionNames[i]) free(_selectionNames[i]);
        if (_selectionValues[i]) free(_selectionValues[i]);
      }
      delete[] _selectionNames;
      delete[] _selectionValues;
    };

    friend class WizardWidget;

};


#endif
