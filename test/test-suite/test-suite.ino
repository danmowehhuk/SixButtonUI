#include <Eventuino.h>
#include <SixButtonUI.h>
#include <sixbuttonui/SubMenuWidget.h>
#include <TestTool.h>

#include "UIConfig.h"

void after() {
  // Return to initial state so memory matches
  helper.reset();
}

void testNavConfigInitialized(TestInvocation* t) {
  t->setName(F("Nav config initialization"));
  t->assert(MODEL.isTitlePmem(), F("Expected a PROGMEM title"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("titleLine should have been 'Main Menu'"));
  t->assert(MODEL.isInteractivePmem(), F("Expected a PROGMEM interactive line"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"), F("interactiveLine should have been 'First'"));
}

void testMenuButtonAtRootLevel(TestInvocation* t) {
  t->setName(F("Menu button toggles thru root menus"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"), F("titleLine should have been 'Settings'"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("titleLine should have been 'Main Menu'"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"), F("interactiveLine should have been 'First'"));
}

void testRootLevelSelectionPreserved(TestInvocation* t) {
  t->setName(F("Returns to previous root menu selection"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("need to start at 'Main Menu'"));
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  helper.pressAndReleaseDown(); // Select 'Date'
  helper.pressAndReleaseMenuBack(); // Switch to 'Main Menu'
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"), F("should have returned to 'Settings'"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("Date"), F("'Date' should still be selected"));
}

void testEnterSelectInSubMenu(TestInvocation* t) {
  t->setName(F("Submenu 'enter' goes to child"));
  helper.pressAndReleaseDown();
  helper.pressAndReleaseSelectEnter();
  t->assert(!MODEL.isTitlePmem(), F("Expected a non-PROGMEM title"));
  t->assertEqual(MODEL.getTitleLine(), F("Second"), F("titleLine should have been 'Second'"));
}

void testBackToSubMenu(TestInvocation* t) {
  t->setName(F("Returns to previous submenu selection"));
  helper.pressAndReleaseDown();
  char* selection = strdup(MODEL.getInteractiveLine());
  helper.pressAndReleaseSelectEnter();
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("should have returned to 'Main Menu'"));
  t->assertEqual(MODEL.getInteractiveLine(), selection, F("Previous selection should still be selected"));
  free(selection);
}

void testSelectorCurrValue(TestInvocation* t) {
  t->setName(F("Selector inits to currValue selection"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"), F("need to start with 'First' selected"));
  helper.pressAndReleaseDown();
  t->assertEqual(MODEL.getInteractiveLine(), F("Second"), F("'Second' should now be selected"));
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(MODEL.getTitleLine(), F("Second"), F("Should have entered 'Second' selector"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("two"), F("Should have had selection 'two'"));
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

  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
