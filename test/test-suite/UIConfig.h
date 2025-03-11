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


ViewModel MODEL(UIElement::Type::UNDEFINED);

void render(ViewModel viewModel) {
  MODEL = static_cast<ViewModel&&>(viewModel);  // Move data into global struct
}

using namespace sixbuttonui;

void loadSelectorModel(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, F("one"), F("buckle"));
  model->setOption(1, F("two"), F("shoe"));
}

NavigationConfig* myNavigationConfig() {
  return new NavigationConfig(
  subMenu()
      ->withTitle(F("Main Menu"))
      ->withMenuItems(
        subMenu()
          ->withTitle(F("First")),
        selector()
          ->withTitle("Second") // leave non-PROGMEM for test
          ->withModelFunction(loadSelectorModel),
        subMenu()
          ->withTitle(F("Third"))
          ->withMenuItems(
            subMenu()
              ->withTitle(F("Low")),
            subMenu()
              ->withTitle(F("Medium")),
            subMenu()
              ->withTitle(F("High"))
          ),
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