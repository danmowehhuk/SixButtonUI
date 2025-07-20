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
                ->onEnter(captureSelectorValue)
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