#include <Eventuino.h>
#include <SixButtonUI.h>
#include <sixbuttonui/SubMenuWidget.h>
#include <TestTool.h>

#include "UIConfig.h"

void testNavConfigInitialized(TestInvocation* t) {
  t->name = STR("Nav config initialization");
  do {
    if (strcmp(MODEL.titleLine, "Main Menu") != 0) {
      t->message = STR("titleLine should have been 'Main Menu'");
      break;
    }
    if (strcmp(MODEL.interactiveLine, "First") != 0) {
      t->message = STR("interactiveLine should have been 'First'");
      break;
    }
    t->success = true;
  } while (false);
}

void testMenuButtonAtRootLevel(TestInvocation* t) {
  t->name = STR("Menu button toggles thru root menus");
  do {
    helper.pressAndReleaseMenuBack();
    if (strcmp(MODEL.titleLine, "Settings") != 0) {
      t->message = STR("titleLine should have been 'Settings'");
      break;
    }
    helper.pressAndReleaseMenuBack();
    if (strcmp(MODEL.titleLine, "Main Menu") != 0) {
      t->message = STR("titleLine should have been 'Main Menu'");
      break;
    }
    if (strcmp(MODEL.interactiveLine, "First") != 0) {
      t->message = STR("interactiveLine should have been 'First'");
      break;
    }
    t->success = true;
  } while (false);
  helper.reset();
}

void testRootLevelSelectionPreserved(TestInvocation* t) {
  t->name = STR("Returns to previous root menu selection");
  do {
    if (strcmp(MODEL.titleLine, "Main Menu") != 0) {
      t->message = STR("need to start at 'Main Menu'");
      break;
    }
    helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
    helper.pressAndReleaseDown(); // Select 'Date'
    helper.pressAndReleaseMenuBack(); // Switch to 'Main Menu'
    helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
    if (strcmp(MODEL.titleLine, "Settings") != 0) {
      t->message = STR("should have returned to 'Settings'");
      break;
    }
    if (strcmp(MODEL.interactiveLine, "Date") != 0) {
      t->message = STR("'Date' should still be selected");
      break;
    }
    t->success = true;
  } while (false);
  helper.reset();
}

void testEnterSelectInSubMenu(TestInvocation* t) {
  t->name = STR("Submenu 'enter' goes to child");
  do {
    helper.pressAndReleaseDown();
    helper.pressAndReleaseSelectEnter();
    if (strcmp(MODEL.titleLine, "Second") != 0) {
      t->message = STR("titleLine should have been 'Second'");
      break;
    }
    t->success = true;
  } while (false);
  helper.reset();
}

void testBackToSubMenu(TestInvocation* t) {
  t->name = STR("Returns to previous submenu selection");
  do {
    helper.pressAndReleaseDown();
    char* selection = strdup(MODEL.interactiveLine);
    helper.pressAndReleaseSelectEnter();
    helper.pressAndReleaseMenuBack();
    if (strcmp(MODEL.titleLine, "Main Menu") != 0) {
      t->message = STR("should have returned to 'Main Menu'");
      break;
    }
    if (strcmp(MODEL.interactiveLine, selection) != 0) {
      t->message = STR("Previous selection should still be selected");
      break;
    }
    helper.reset();
    if (strcmp(MODEL.interactiveLine, "First") != 0) {
      t->message = STR("Selection should have reset to 'First'");
      break;
    }
    free(selection);
    t->success = true;
  } while (false);
}

void testSelectorCurrValue(TestInvocation* t) {
  t->name = STR("Selector inits to currValue selection");
  do {
    if (strcmp(MODEL.interactiveLine, "First") != 0) {
      t->message = STR("need to start with 'First' selected");
      break;
    }
    helper.pressAndReleaseDown();
    helper.pressAndReleaseSelectEnter();
    if (strcmp(MODEL.titleLine, "Second") != 0) {
      t->message = STR("Should have entered 'Second' selector");
      break;
    }
    if (strcmp(MODEL.interactiveLine, "two") != 0) {
      t->message = STR("Should have had selection 'two'");
      break;
    }
    t->success = true;
  } while (false);
  helper.reset();
}

Eventuino evt;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  evt.addEventSource(&sixButtonUI);
  evt.begin();

  TestFunction tests[] = {

    testNavConfigInitialized,
    testMenuButtonAtRootLevel,
    testRootLevelSelectionPreserved,
    testEnterSelectInSubMenu,
    testBackToSubMenu,
    testSelectorCurrValue

  };
  int numTests = 6;

  runTestSuite(tests, numTests, true);
}

void loop() {}
