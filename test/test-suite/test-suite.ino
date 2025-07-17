#include <Eventuino.h>
#include <SixButtonUI.h>
#include <TestTool.h>

#include "UIConfig.h"

using namespace sixbuttonui;

void testInit(TestInvocation* t) {
  t->setName(F("test initialization"));
  t->assert(true, F("test"));
}

void testUIElementConfigRAM(TestInvocation* t) {
  t->setName(F("UIElement configuration in RAM"));
  t->assert(helper.goToNamedElement(F("RAM-selector")), F("Element not found"));
  t->assert(!MODEL.isTitlePmem(), F("Expected a non-PROGMEM title"));
  t->assertEqual(MODEL.getTitleLine(), F("RAM-selector"));
  t->assert(!MODEL.isInstructionPmem(), F("Expected a non-PROGMEM instruction"));
  t->assertEqual(MODEL.getInstructionLine(), F("Press"));
  t->assert(!MODEL.isFooterPmem(), F("Expected a non-PROGMEM footer"));
  t->assertEqual(MODEL.getFooterLine(), F("Enter"));
}

void testUIElementConfigPmem(TestInvocation* t) {
  t->setName(F("UIElement configuration in PROGMEM"));
  t->assert(helper.goToNamedElement(F("PMEM-selector")), F("Element not found"));
  t->assert(MODEL.isTitlePmem(), F("Expected a PROGMEM title"));
  t->assertEqual(MODEL.getTitleLine_P(), F("PMEM-selector"));
  t->assert(MODEL.isInstructionPmem(), F("Expected a PROGMEM instruction"));
  t->assertEqual(MODEL.getInstructionLine_P(), F("Drink"));
  t->assert(MODEL.isFooterPmem(), F("Expected a PROGMEM footer"));
  t->assertEqual(MODEL.getFooterLine_P(), F("Back"));
}

void testMenuButtonAtRootLevel(TestInvocation* t) {
  t->setName(F("Menu button toggles thru root menus"));
  t->assert(helper.goToNamedElement(F("Main Menu")), F("Element not found"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"));
}

void testRootLevelSelectionPreserved(TestInvocation* t) {
  t->setName(F("Returns to previous root menu selection"));
  t->assert(helper.goToNamedElement(F("Main Menu")), F("Element not found"));
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  helper.pressAndReleaseDown(); // Select 'Date'
  t->assertEqual(MODEL.getInteractiveLine_P(), F("Date"), F("Switch to Date failed"));
  helper.pressAndReleaseMenuBack(); // Switch to 'Main Menu'
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  t->assertEqual(MODEL.getTitleLine_P(), F("Settings"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("Date"));
}

void testBackToSubMenu(TestInvocation* t) {
  t->setName(F("Returns to previous submenu selection"));
  t->assert(helper.goToNamedElement(F("Main Menu")), F("Element not found"));
  helper.pressAndReleaseDown();
  char* selection = strdup(MODEL.getInteractiveLine_P());
  helper.pressAndReleaseSelectEnter();
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("Should have returned to 'Main Menu'"));
  t->assertEqual(MODEL.getInteractiveLine_P(), selection, F("Previous selection should still be selected"));
  free(selection);
}

void testSubMenuWidget(TestInvocation* t) {
  t->setName(F("SubMenu navigation functionality"));
  t->assert(helper.goToNamedElement(F("Main Menu")), F("Element not found"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("First"), F("need to start with 'First' selected"));
  helper.pressAndReleaseDown();
  t->assertEqual(MODEL.getInteractiveLine_P(), F("Second"), F("'Second' should now be selected"));
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(MODEL.getTitleLine_P(), F("Second"), F("Should have entered 'Second' selector"));
  helper.pressAndReleaseMenuBack();
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testSelectorWidget_ramOptions(TestInvocation* t) {
  t->setName(F("Selector core functionality RAM options"));
  t->assert(helper.goToNamedElement(F("Third")), F("Element not found"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Third"), F("Should have entered 'Third' selector"));
  t->assertEqual(MODEL.getInteractiveLine(), F("two"));
  helper.pressAndReleaseUp();
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(capturedSelectionName, F("one"), F("Captured selectionName should have been 'one'"));
  t->assertEqual(capturedSelectionValue, F("buckle"), F("Captured selectionValue should have been 'buckle'"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testSelectorWidget_pmemOptions(TestInvocation* t) {
  t->setName(F("Selector core functionality PMEM options"));
  t->assert(helper.goToNamedElement(F("Second")), F("Element not found"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Second"), F("Should have entered 'Second' selector"));
  t->assertEqual(MODEL.getInteractiveLine_P(), F("two"));
  helper.pressAndReleaseUp();
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(capturedSelectionName, F("one"), F("Captured selectionName should have been 'one'"));
  t->assertEqual(capturedSelectionValue, F("buckle"), F("Captured selectionValue should have been 'buckle'"));
  t->assertEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testTextInputWidget(TestInvocation* t) {
  t->setName(F("TextInput core functionality"));
  t->assert(helper.goToNamedElement(F("TextBox")), F("Element not found"));
  t->assertEqual(MODEL.getTitleLine_P(), F("TextBox"), F("Should have entered 'TextBox'"));
  t->assertEqual(MODEL.getInteractiveLine(), F("bar"));
  helper.pressAndReleaseDown();
  t->assertEqual(MODEL.getInteractiveLine(), F("bas"));
  helper.pressAndReleaseSelectEnter();
  t->assertEqual(capturedText, F("bas"), F("Captured text should have been 'bas'"));
  t->assertEqual(MODEL.getTitleLine_P(), F("First"), F("Should have returned to parent"));
}



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

Eventuino evt;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  evt.addEventSource(sixButtonUI);
  evt.begin();

  TestFunction tests[] = {
    
    testInit,
    testUIElementConfigRAM,
    testUIElementConfigPmem,
    testMenuButtonAtRootLevel,
    testSelectorWidget_ramOptions,
    testSelectorWidget_pmemOptions,
    testTextInputWidget,
    testRootLevelSelectionPreserved,
    testBackToSubMenu,
    testSubMenuWidget

  };

  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
