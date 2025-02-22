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
    t->success = true;
  } while (false);
}

void testRootLevelMenuButton(TestInvocation* t) {
  t->name = STR("Menu button toggles thru root menus");
  do {
    helper.goHome();
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
    t->success = true;
  } while (false);
}

void testSubMenuEnterSelect(TestInvocation* t) {
  t->name = STR("Submenu 'enter' goes to child");
  do {
    helper.goHome();
    if (strcmp(MODEL.interactiveLine, "First") != 0) {
      t->message = STR("test needs to start with 'First' selected");
      break;
    }
    helper.pressAndReleaseDown();
    helper.pressAndReleaseSelectEnter();
    if (strcmp(MODEL.titleLine, "Second") != 0) {
      t->message = STR("titleLine should have been 'Second'");
      break;
    }
    t->success = true;
  } while (false);
  // return to original state so free memory matches
  helper.goHome();
}

void testBackToSubMenu(TestInvocation* t) {
  t->name = STR("Returns to previous submenu selection");
  do {
    helper.goHome();
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
    free(selection);
    t->success = true;
  } while (false);
  // return to original state so free memory matches
  helper.goHome();
}

void testRootLevelSelectionPreserved(TestInvocation* t) {
  t->name = STR("Returns to previous root menu selection");
  do {
    helper.goHome();
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
  // return to original state so free memory matches
  helper.goHome();
}


Eventuino evt;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  evt.addEventSource(&sixButtonUI);
  evt.begin();

  TestFunction tests[] = {

    testNavConfigInitialized,
    testRootLevelMenuButton,
    testSubMenuEnterSelect,
    testBackToSubMenu,
    testRootLevelSelectionPreserved

  };
  int numTests = 5;

  runTestSuite(tests, numTests, true);
}

void loop() {}
