#ifndef __test_UIConfig_h
#define __test_UIConfig_h


#define UP_BUTTON_PIN    6
#define DOWN_BUTTON_PIN  4
#define LEFT_BUTTON_PIN  3
#define RIGHT_BUTTON_PIN 7
#define MENU_BUTTON_PIN  2
#define ENTER_BUTTON_PIN 5

#include "SixButtonUITestHelper.h"
#include <SixButtonUI.h>
#include <sixbuttonui/Strings.h>

using namespace SixButtonUIStrings;
using namespace sixbuttonui;

ViewModel MODEL(UIElement::Type::UNDEFINED);

void render(ViewModel viewModel) {
  MODEL = static_cast<ViewModel&&>(viewModel);  // Move data into global struct
}

void loadSelectorModel(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, F("one"), F("buckle"));
  model->setOption(1, F("two"), F("shoe"));
}

void loadSelectorModelRAM(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, "one", "buckle");
  model->setOption(1, "two", "shoe");
}

void initializeTextBox(TextInputModel* model, void* state) {
  model->setInitialValue(F("bar"));
}

void loadBasicComboBoxModel(SelectorModel* model, void* state) {
  const char* searchPrefix = model->getSearchPrefix();
  if (!searchPrefix || searchPrefix[0] == '\0') {
    model->setNumOptions(2);
    model->setOptionRaw(0, PSTR("a"), true, PSTR("a"), true);
    model->setOptionRaw(1, "b", false, nullptr, false);
  } else if (strcmp(searchPrefix, "b") == 0) {
    model->setNumOptions(2);
    model->setOptionRaw(0, "e", false, PSTR("be"), true);
    model->setOptionRaw(1, PSTR("r"), true, nullptr, false);
  } else {
    model->setNumOptions(0);
  }
}

void loadPreloadedComboBoxModel(SelectorModel* model, void* state) {
  if (!model->getSearchPrefix()) { // not initialized
    model->setCurrValue("be");
    model->setNumOptions(1);
    model->setOption(0, F("be"), F("be"));
  } else {
    loadBasicComboBoxModel(model, state);
  }
}

char* capturedSelectionName = nullptr;
char* capturedSelectionValue = nullptr;
void* captureSelectorValue(const char* selectionName, bool namePmem, const char* selectionValue, bool valuePmem, void* state) {
  capturedSelectionName = namePmem ? strdup_P (selectionName) : strdup(selectionName);
  capturedSelectionValue = valuePmem ? strdup_P(selectionValue) : strdup(selectionValue);
  return state;
}

char* capturedText = nullptr;
void* captureTextValue(const char* value, void* state) {
  capturedText = strdup(value);
  return state;
}

char** capturedWizardValues = nullptr;
uint8_t capturedWizardNumSelections = 0;
void* captureWizardValues(const char** selectionValues, uint8_t numSelections, void* state) {
  if (capturedWizardValues) {
    for (uint8_t i = 0; i < capturedWizardNumSelections; i++) {
      if (capturedWizardValues[i]) free(capturedWizardValues[i]);
    }
    delete[] capturedWizardValues;
    capturedWizardValues = nullptr;
  }
  capturedWizardNumSelections = numSelections;
  capturedWizardValues = new char*[numSelections]();
  for (uint8_t i = 0; i < numSelections; i++) {
    capturedWizardValues[i] = selectionValues[i] ? strdup(selectionValues[i]) : nullptr;
  }
  return state;
}

void loadWizardModel(WizardModel* model, void* state) {
  // No initial values for the step selectors,
  // any step that is never viewed will have value nullptr
}

void loadWizardModelFull(WizardModel* model, void* state) {
  model->setStepInitialValue(0, F("shoe"));
  model->setStepInitialValue(1, "shoe");
  model->setStepInitialValue(2, F("buckle"));
}

SixButtonUI* sixButtonUI = nullptr;
SixButtonUI* initSixButtonUI() {
  sixButtonUI = new SixButtonUI(
        UP_BUTTON_PIN,    DOWN_BUTTON_PIN,
        LEFT_BUTTON_PIN,  RIGHT_BUTTON_PIN,
        MENU_BUTTON_PIN,  ENTER_BUTTON_PIN,
        render,
        NavigationConfig(
          subMenu()
            ->withTitle(F("Main Menu"))
            ->withInstruction(F("Press"))
            ->withFooter(F("Back"))
            ->withMenuItems(
              subMenu()
                ->withTitle(F("First"))
                ->withMenuItems(
                  selector()
                    ->withTitle("RAM-selector") // non-PROGMEM for test
                    ->withInstruction("Press")
                    ->withFooter("Enter")
                    ->withModelFunction(loadSelectorModel)
                    ->onEnter(captureSelectorValue),
                  selector()
                    ->withTitle(F("PMEM-selector")) // PROGMEM for test
                    ->withInstruction(F("Drink"))
                    ->withFooter(F("Back"))
                    ->withModelFunction(loadSelectorModel)
                    ->onEnter(captureSelectorValue),
                  textInput()
                    ->withTitle(F("TextBox"))
                    ->withInitialValue(F("foo")) // model function overrides this
                    ->withModelFunction(initializeTextBox)
                    ->onEnter(captureTextValue),
                  comboBox()
                    ->withTitle(F("DefaultComboBox"))
                    ->withModelFunction(loadBasicComboBoxModel)
                    ->onEnter(captureSelectorValue),
                  comboBox()
                    ->withTitle(F("PreloadedComboBox"))
                    ->withModelFunction(loadPreloadedComboBoxModel)
                    ->onEnter(captureSelectorValue)
                ),
              selector()
                ->withTitle(F("Second"))
                ->withInstruction(F("Check"))
                ->withFooter(F("Enter"))
                ->withModelFunction(loadSelectorModel)
                ->onEnter(captureSelectorValue),
              selector()
                ->withTitle(F("Third"))
                ->withInstruction(F("Check"))
                ->withFooter(F("Enter"))
                ->withModelFunction(loadSelectorModelRAM)
                ->onEnter(captureSelectorValue),
              wizard()
                ->withTitle(F("EmptyWizard"))
                ->withInstruction(F("Setup"))
                ->withFooter(F("Enter"))
                ->withModelFunction(loadWizardModel)
                ->withSteps(
                  selector()
                    ->withTitle(F("1st Step"))
                    ->withModelFunction(loadSelectorModel),
                  selector()
                    ->withTitle(F("Middle Step"))
                    ->withModelFunction(loadSelectorModel),
                  selector()
                    ->withTitle(F("Last Step"))
                    ->withModelFunction(loadSelectorModel)
                )
                ->onEnter(captureWizardValues),
              wizard()
                ->withTitle(F("FullWizard"))
                ->withInstruction(F("Setup"))
                ->withFooter(F("Enter"))
                ->withModelFunction(loadWizardModelFull)
                ->withSteps(
                  selector()
                    ->withTitle(F("1st Step"))
                    ->withModelFunction(loadSelectorModel),
                  selector()
                    ->withTitle(F("Middle Step"))
                    ->withModelFunction(loadSelectorModel),
                  selector()
                    ->withTitle(F("Last Step"))
                    ->withModelFunction(loadSelectorModel)
                )
                ->onEnter(captureWizardValues)
              ),
          subMenu()
            ->withTitle(F("Settings"))
            ->withMenuItems(
              subMenu()
                ->withTitle(F("Clock")),
              subMenu()
                ->withTitle(F("Date"))
            )
        )
  );
  return sixButtonUI;
}

SixButtonUITestHelper helper(initSixButtonUI());


#endif
