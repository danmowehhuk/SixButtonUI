#ifndef __test_UIConfig_h
#define __test_UIConfig_h


#include "ButtonTestHelper.h"
#include <SixButtonUI.h>

#define UP_BUTTON_PIN    6
#define DOWN_BUTTON_PIN  4
#define LEFT_BUTTON_PIN  3
#define RIGHT_BUTTON_PIN 7
#define MENU_BUTTON_PIN  2
#define ENTER_BUTTON_PIN 5

char* strdup_P(const char* str_P) {
  if (!str_P) return nullptr;
  size_t len = strlen_P(str_P);
  char* str = new char[len + 1];
  if (str) {
    strncpy_P(str, str_P, len);
    str[len] = '\0';
  }
  return str;
}

ViewModel MODEL(UIElement::Type::UNDEFINED);
void render(ViewModel viewModel) {
  MODEL = static_cast<ViewModel&&>(viewModel);  // Move data into global struct
}

char* capturedText = nullptr;
void* captureTextValue(const char* value, void* state) {
  capturedText = strdup(value);
  return state;
}

char* capturedSelectionName = nullptr;
char* capturedSelectionValue = nullptr;
void* captureSelectorValue(const char* selectionName, bool namePmem, const char* selectionValue, bool valuePmem, void* state) {
  capturedSelectionName = namePmem ? strdup_P (selectionName) : strdup(selectionName);
  capturedSelectionValue = valuePmem ? strdup_P(selectionValue) : strdup(selectionValue);
  return state;
}

void loadSelectorModel(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, F("one"), F("buckle"));
  model->setOption(1, F("two"), F("shoe"));
}

void initializeTextBox(TextInputModel* model, void* state) {
  model->initialize(F("bar"));
}

using namespace sixbuttonui;

NavigationConfig* myNavigationConfig() {
  return new NavigationConfig(
  subMenu()
      ->withTitle(F("Main Menu"))
      ->withInstruction(F("Press"))
      ->withFooter(F("Back"))
      ->withMenuItems(
        subMenu()
          ->withTitle(F("First")),
        selector()
          ->withTitle("Second") // leave non-PROGMEM for test
          ->withInstruction("Check")
          ->withFooter("Enter")
          ->withModelFunction(loadSelectorModel)
          ->onEnter(captureSelectorValue),
        textInput()
          ->withTitle(F("Third"))
          ->withInitialValue(F("foo")) // model function overrides this
          ->withModelFunction(initializeTextBox)
          ->onEnter(captureTextValue),
        subMenu()
          ->withTitle(F("Fourth"))
      ),
  subMenu()
      ->withTitle(F("Settings"))
      ->withMenuItems(
        subMenu()
          ->withTitle(F("Clock")),
        subMenu()
          ->withTitle(F("Date"))
      )
  );
};

SixButtonUI sixButtonUI(
      UP_BUTTON_PIN, DOWN_BUTTON_PIN, LEFT_BUTTON_PIN,
      RIGHT_BUTTON_PIN, MENU_BUTTON_PIN, ENTER_BUTTON_PIN,
      myNavigationConfig(), render
);

ButtonTestHelper helper(&sixButtonUI);


#endif