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


ViewModel MODEL("", 0, "", 0, "", 0, "", 0);

void render(ViewModel* viewModel) {
  MODEL = *viewModel;  // Copy data into global struct
}

using namespace sixbuttonui;

void loadSelectorModel(SelectorElement::Model* model, void* state) {
  model->numOptions = 0;
  // model->currValue = "shoe";

  // static const char* names[] = { "one", "two" };
  // model->optionNames = names;
  // static const char* values[] = { "buckle", "shoe" };
  // model->optionValues = values;

  // model->optionNames = new char*[numOptions];
  // model->optionValues = new char*[numOptions];

}

NavigationConfig* config = new NavigationConfig(
  subMenu()
      ->withTitle("Main Menu")
      ->withMenuItems(
        subMenu()
          ->withTitle("First"),
        selector()
          ->withTitle("Second"),
          // ->withModelFunction(loadSelectorModel),
        subMenu()
          ->withTitle("Third")
          ->withMenuItems(
            subMenu()
              ->withTitle("Low"),
            subMenu()
              ->withTitle("Medium"),
            subMenu()
              ->withTitle("High")
          ),
        subMenu()
          ->withTitle("Fourth")
      ),
  subMenu()
      ->withTitle("Settings")
      ->withMenuItems(
        subMenu()
          ->withTitle("Clock"),
        subMenu()
          ->withTitle("Date")
      )
);

SixButtonUI sixButtonUI(
      UP_BUTTON_PIN, DOWN_BUTTON_PIN, LEFT_BUTTON_PIN,
      RIGHT_BUTTON_PIN, MENU_BUTTON_PIN, ENTER_BUTTON_PIN,
      config, render
);

ButtonTestHelper helper(&sixButtonUI);


#endif