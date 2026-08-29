#ifndef _test_mcp_UIConfig_h
#define _test_mcp_UIConfig_h


#define UP_BUTTON_PIN    6
#define DOWN_BUTTON_PIN  4
#define LEFT_BUTTON_PIN  3
#define RIGHT_BUTTON_PIN 7
#define MENU_BUTTON_PIN  2
#define ENTER_BUTTON_PIN 5

#include <SixButtonUI.h>
#include <SixButtonUITestHelper.h>

using namespace sixbuttonui;

enum TestElement : uint8_t {
  MAIN_MENU = 1,
  ITEM_SELECTOR
};

void loadItemOptions(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("alpha");
  model->setOption(0, "alpha", "a");
  model->setOption(1, "beta", "b");
}

char* capturedSelectionValue = nullptr;
void* captureSelection(const char* name, bool namePmem, const char* value, bool valuePmem, void* state) {
  if (capturedSelectionValue) free(capturedSelectionValue);
  capturedSelectionValue = valuePmem ? strdup_P(value) : strdup(value);
  return state;
}

ViewModel MODEL(UIElement::Type::UNDEFINED);
void render(ViewModel viewModel) {
  MODEL = static_cast<ViewModel&&>(viewModel);
}

SixButtonUI* sixButtonUI = nullptr;
NavigationConfig* navConfig = nullptr;

SixButtonUI* initSixButtonUI() {
  navConfig = new NavigationConfig(
    subMenu(TestElement::MAIN_MENU)
      ->withTitle("Main Menu")
      ->withMenuItems(
        selector(TestElement::ITEM_SELECTOR)
          ->withTitle("Items")
          ->withModelFunction(loadItemOptions)
          ->onEnter(captureSelection)
      )
  );

  sixButtonUI = new SixButtonUI(
        UP_BUTTON_PIN,    DOWN_BUTTON_PIN,
        LEFT_BUTTON_PIN,  RIGHT_BUTTON_PIN,
        MENU_BUTTON_PIN,  ENTER_BUTTON_PIN,
        render,
        navConfig
  );
  return sixButtonUI;
}

SixButtonUITestHelper helper(initSixButtonUI());


#endif
