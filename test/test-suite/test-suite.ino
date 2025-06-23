#include <Eventuino.h>
#include <SixButtonUI.h>
#include <TestTool.h>
#include "DummyElement.h"

#define UP_BUTTON_PIN    6
#define DOWN_BUTTON_PIN  4
#define LEFT_BUTTON_PIN  3
#define RIGHT_BUTTON_PIN 7
#define MENU_BUTTON_PIN  2
#define ENTER_BUTTON_PIN 5

void render(ViewModel viewModel) {
}

SixButtonUI sixButtonUI(
      UP_BUTTON_PIN,    DOWN_BUTTON_PIN,
      LEFT_BUTTON_PIN,  RIGHT_BUTTON_PIN,
      MENU_BUTTON_PIN,  ENTER_BUTTON_PIN,
      render,
      NavigationConfig(
        new DummyElement()
      )
);

void testInit(TestInvocation* t) {
  t->setName(F("test initialization"));
  t->assert(true, F("test"));
}

Eventuino evt;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  evt.addEventSource(&sixButtonUI);
  evt.begin();

  TestFunction tests[] = {
    
    testInit

  };

  runTestSuiteShowMem(tests, nullptr, nullptr);
}

void loop() {}
