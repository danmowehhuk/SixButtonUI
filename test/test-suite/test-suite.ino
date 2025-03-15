#include <Eventuino.h>
#include <SixButtonUI.h>
#include <sixbuttonui/SubMenuWidget.h>
#include <TestTool.h>

#include "UIConfig.h"

void after() {
  // Return to initial state so memory matches
  helper.reset();
  if (capturedText) free(capturedText);
  if (capturedSelectionName) free(capturedSelectionName);
  if (capturedSelectionValue) free(capturedSelectionValue);
  capturedText = nullptr;
  capturedSelectionName = nullptr;
  capturedSelectionValue = nullptr;
}

void testNavConfigInitialized(TestInvocation* t) {
  t->setName(F("Nav config initialization"));
  t->assert(MODEL.isTitlePmem(), F("Expected a PROGMEM title"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"));
  t->assert(MODEL.isInstructionPmem(), F("Expected a PROGMEM instruction"));
  t->assertEqual(MODEL.getInstructionLine_P(), F("Press"));
  t->assert(MODEL.isInteractivePmem(), F("Expected a PROGMEM interactive line"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"));
  t->assert(MODEL.isFooterPmem(), F("Expected a PROGMEM footer"));
  t->assertEqual(MODEL.getFooterLine_P(), F("Back"));
}

void testNonPmemBaseFields(TestInvocation* t) {
  t->setName(F("non-PROGMEM fields from UIElement populated"));
  helper.pressAndReleaseDown();
  t->assert(!MODEL.isInteractivePmem(), F("Expected a non-PROGMEM interactive line"));
  t->assertEqual(MODEL.getInteractiveLine(), F("Second"), F("'Second' should now be selected"));
  helper.pressAndReleaseSelectEnter();
  t->assert(!MODEL.isTitlePmem(), F("Expected a non-PROGMEM title"));
  t->assertEqual(MODEL.getTitleLine(), F("Second"));
  t->assert(!MODEL.isInstructionPmem(), F("Expected a non-PROGMEM instruction"));
  t->assertEqual(MODEL.getInstructionLine(), F("Check"));
  t->assert(!MODEL.isFooterPmem(), F("Expected a non-PROGMEM footer"));
  t->assertEqual(MODEL.getFooterLine(), F("Enter"));
}

void testMenuButtonAtRootLevel(TestInvocation* t) {
  t->setName(F("Menu button toggles thru root menus"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"));
}

void testRootLevelSelectionPreserved(TestInvocation* t) {
  t->setName(F("Returns to previous root menu selection"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"));
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  helper.pressAndReleaseDown(); // Select 'Date'
  helper.pressAndReleaseMenuBack(); // Switch to 'Main Menu'
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("Date"));
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

void testSelectorWidget(TestInvocation* t) {
  t->setName(F("Selector core functionality"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"), F("need to start with 'First' selected"));
  helper.pressAndReleaseDown();
  t->assertEqual(MODEL.getInteractiveLine(), F("Second"), F("'Second' should now be selected"));
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(MODEL.getTitleLine(), F("Second"), F("Should have entered 'Second' selector"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("two"));
  helper.pressAndReleaseUp();
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(capturedSelectionName, F("one"), F("Captured selectionName should have been 'one'"));
  t->assertEqual(capturedSelectionValue, F("buckle"), F("Captured selectionValue should have been 'buckle'"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testTextInputWidget(TestInvocation* t) {
  t->setName(F("TextInput core functionality"));
  helper.pressAndReleaseDown();
  helper.pressAndReleaseDown();
  t->assert(MODEL.isInteractivePmem(), F("Expected a PROGMEM interactive line"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("Third"), F("'Third' should now be selected"));
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(MODEL.getTitleLine_P(), F("Third"));
  t->assertEqual(MODEL.getInteractiveLine(), F("bar"));
  helper.pressAndReleaseDown();
  t->assertEqual(MODEL.getInteractiveLine(), F("bas"));
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(capturedText, F("bas"), F("Captured text should have been 'bas'"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("Should have returned to parent"));
}

Eventuino evt;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  evt.addEventSource(&sixButtonUI);
  evt.begin();

  TestFunction tests[] = {

    testNavConfigInitialized,
    testSelectorWidget,
    testTextInputWidget,
    testNonPmemBaseFields,
    testMenuButtonAtRootLevel,
    testRootLevelSelectionPreserved,
    testBackToSubMenu

  };

  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
