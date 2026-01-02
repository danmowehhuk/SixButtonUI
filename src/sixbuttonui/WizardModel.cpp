#include "WizardModel.h"

void WizardModel::setStepInitialValue(const uint8_t step, const char* value) {
  setStepInitialValueRaw(step, value, false);
}

void WizardModel::setStepInitialValue(const uint8_t step, const __FlashStringHelper* value) {
  setStepInitialValueRaw(step, reinterpret_cast<const char*>(value), true);
}

void WizardModel::setStepInitialValueRaw(const uint8_t step, const char* value, bool isPmem) {
  if (_selectionValues[step]) free(_selectionValues[step]);
  // Always allocate because WizardModel needs to own these values
  if (value) {
    _selectionValues[step] = isPmem ? strdup_P(value) : strdup(value);
  } else {
    _selectionValues[step] = nullptr;
  }
}

void WizardModel::setStepTitle(const char* title, bool allocate) {
  setStepTitleRaw(title, false, allocate);
}

void WizardModel::setStepTitle(const __FlashStringHelper* title) {
  setStepTitleRaw(reinterpret_cast<const char*>(title), true, false);
}

void WizardModel::setStepTitleRaw(const char* title, bool isPmem, bool allocate) {
  if (_stepTitle && _ownsStepTitle) free(_stepTitle);
  if (!title || isPmem || !allocate) {
    _stepTitle = title;
    _ownsStepTitle = false;
  } else {
    _stepTitle = strdup(title);
    _ownsStepTitle = true;
  }
  _stepTitlePmem = isPmem;
}

void WizardModel::setInitialStep(const uint8_t step) {
  if (step >= _numSteps) return;
  _hasPrev = step > 0;
  _hasNext = step < _numSteps - 1;
  _currStep = step;
}
