#include <Eventuino.h>
#include <SixButtonUI.h>
#include <TestTool.h>

#include "UIConfig.h"

using namespace sixbuttonui;

void testInit(TestInvocation* t) {
  t->setName(F("test initialization"));
  t->verify(true, F("test"));
}

void testUIElementConfigRAM(TestInvocation* t) {
  t->setName(F("UIElement configuration in RAM"));
  t->verify(helper.goToElementById(TestElement::RAM_SELECTOR), F("Element not found"));
  t->verify(!MODEL.isTitlePmem(), F("Expected a non-PROGMEM title"));
  t->verifyEqual(MODEL.getTitleLine(), F("RAM-selector"));
  t->verify(!MODEL.isInstructionPmem(), F("Expected a non-PROGMEM instruction"));
  t->verifyEqual(MODEL.getInstructionLine(), F("Press"));
  t->verify(!MODEL.isFooterPmem(), F("Expected a non-PROGMEM footer"));
  t->verifyEqual(MODEL.getFooterLine(), F("Enter"));
}

void testUIElementConfigPmem(TestInvocation* t) {
  t->setName(F("UIElement configuration in PROGMEM"));
  t->verify(helper.goToElementById(TestElement::PMEM_SELECTOR), F("Element not found"));
  t->verify(MODEL.isTitlePmem(), F("Expected a PROGMEM title"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("PMEM-selector"));
  t->verify(MODEL.isInstructionPmem(), F("Expected a PROGMEM instruction"));
  t->verifyEqual(MODEL.getInstructionLine_P(), F("Drink"));
  t->verify(MODEL.isFooterPmem(), F("Expected a PROGMEM footer"));
  t->verifyEqual(MODEL.getFooterLine_P(), F("Back"));
}

void testMenuButtonAtRootLevel(TestInvocation* t) {
  t->setName(F("Menu button toggles thru root menus"));
  t->verify(helper.goToElementById(TestElement::MAIN_MENU), F("Element not found"));
  helper.pressAndReleaseMenuBack();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Settings"));
  helper.pressAndReleaseMenuBack();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"));
  helper.pressAndReleaseMenuBack();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Settings"));
}

void testRootLevelSelectionPreserved(TestInvocation* t) {
  t->setName(F("Returns to previous root menu selection"));
  t->verify(helper.goToElementById(TestElement::MAIN_MENU), F("Element not found"));
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  helper.pressAndReleaseDown(); // Select 'Date'
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("Date"), F("Switch to Date failed"));
  helper.pressAndReleaseMenuBack(); // Switch to 'Main Menu'
  helper.pressAndReleaseMenuBack(); // Switch to 'Settings'
  t->verifyEqual(MODEL.getTitleLine_P(), F("Settings"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("Date"));
}

void testBackToSubMenu(TestInvocation* t) {
  t->setName(F("Returns to previous submenu selection"));
  t->verify(helper.goToElementById(TestElement::MAIN_MENU), F("Element not found"));
  helper.pressAndReleaseDown();
  char* selection = strdup(MODEL.getInteractiveLine_P());
  helper.pressAndReleaseSelectEnter();
  helper.pressAndReleaseMenuBack();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"), F("Should have returned to 'Main Menu'"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), selection, F("Previous selection should still be selected"));
  free(selection);
}

void testSubMenuWidget(TestInvocation* t) {
  t->setName(F("SubMenu navigation functionality"));
  t->verify(helper.goToElementById(TestElement::MAIN_MENU), F("Element not found"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("First"), F("need to start with 'First' selected"));
  helper.pressAndReleaseDown();
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("Second"), F("'Second' should now be selected"));
  helper.pressAndReleaseSelectEnter();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Second"), F("Should have entered 'Second' selector"));
  helper.pressAndReleaseMenuBack();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testSelectorWidget_ramOptions(TestInvocation* t) {
  t->setName(F("Selector core functionality RAM options"));
  t->verify(helper.goToElementById(TestElement::THIRD), F("Element not found"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("Third"), F("Should have entered 'Third' selector"));
  t->verifyEqual(MODEL.getInteractiveLine(), F("two"));
  helper.pressAndReleaseUp();
  helper.pressAndReleaseSelectEnter();
  t->verifyEqual(capturedSelectionName, F("one"), F("Captured selectionName should have been 'one'"));
  t->verifyEqual(capturedSelectionValue, F("buckle"), F("Captured selectionValue should have been 'buckle'"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testSelectorWidget_pmemOptions(TestInvocation* t) {
  t->setName(F("Selector core functionality PMEM options"));
  t->verify(helper.goToElementById(TestElement::SECOND), F("Element not found"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("Second"), F("Should have entered 'Second' selector"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("two"));
  helper.pressAndReleaseUp();
  helper.pressAndReleaseSelectEnter();
  t->verifyEqual(capturedSelectionName, F("one"), F("Captured selectionName should have been 'one'"));
  t->verifyEqual(capturedSelectionValue, F("buckle"), F("Captured selectionValue should have been 'buckle'"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"), "Should have returned to parent");
}

void testTextInputWidget(TestInvocation* t) {
  t->setName(F("TextInput core functionality"));
  t->verify(helper.goToElementById(TestElement::TEXTBOX), F("Element not found"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("TextBox"), F("Should have entered 'TextBox'"));
  t->verifyEqual(MODEL.getInteractiveLine(), F("bar"));
  helper.pressAndReleaseDown();
  t->verifyEqual(MODEL.getInteractiveLine(), F("bas"));
  helper.pressAndReleaseSelectEnter();
  t->verifyEqual(capturedText, F("bas"), F("Captured text should have been 'bas'"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("First"), F("Should have returned to parent"));
}

void testTextInputWidget_cursor(TestInvocation* t) {
  t->setName(F("TextInput cursor functionality"));
  t->verify(helper.goToElementById(TestElement::TEXTBOX), F("Element not found"));
  t->verifyEqual(MODEL.getInteractiveLine(), F("bar"));
  t->verify(MODEL.cursorPosition == 2, F("Expected cursor at 2"));
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getInteractiveLine(), F("bar "));
  t->verify(MODEL.cursorPosition == 3, F("Expected cursor at 3"));
  helper.longPressLeft();
  t->verifyEqual(MODEL.getInteractiveLine(), F(" "));
  t->verify(MODEL.cursorPosition == 0, F("Expected cursor at 0"));
  helper.pressAndReleaseLeft();
  t->verify(MODEL.cursorPosition == 0, F("Expected cursor still at 0"));
}

void testDefaultComboBoxWidget(TestInvocation* t) {
  t->setName(F("ComboBox core functionality"));
  t->verify(helper.goToElementById(TestElement::DEFAULT_COMBO_BOX), F("Element not found"));
  t->verifyEqual(MODEL.getInteractiveLine(), F("a"), F("Expected 'a' on initial load"));
  t->verify(MODEL.isSelectable, F("Expected 'a' to be selectable"));
  t->verify(MODEL.cursorPosition == 0, F("Expected cursor at 0"));
  helper.pressAndReleaseLeft(); // no change, already max left
  t->verifyEqual(MODEL.getInteractiveLine(), F("a"), F("Expected no change onLeft"));
  t->verify(MODEL.cursorPosition == 0, F("Expected cursor still at 0"));
  helper.pressAndReleaseDown();
  t->verifyEqual(MODEL.getInteractiveLine(), F("b"));
  t->verify(!MODEL.isSelectable, F("Expected 'b' NOT to be selectable"));
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getInteractiveLine(), F("be"));
  t->verify(MODEL.isSelectable, F("Expected 'be' to be selectable"));
  t->verify(MODEL.cursorPosition == 1, F("Expected cursor at 1"));
  helper.pressAndReleaseLeft(); 
  t->verifyEqual(MODEL.getInteractiveLine(), F("b"), F("Should have retained 'b'"));
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getInteractiveLine(), F("be"));
  helper.longPressLeft();
  t->verifyEqual(MODEL.getInteractiveLine(), F("a"), F("Expected 'a' after long-hold left"));
  t->verify(MODEL.isSelectable, F("Expected 'a' to be selectable after long-hold left"));
  helper.pressAndReleaseDown();
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getInteractiveLine(), F("be"));
  helper.pressAndReleaseRight(); // no change, already max right
  t->verifyEqual(MODEL.getInteractiveLine(), F("be"));
  t->verify(MODEL.cursorPosition == 1, F("Expected cursor still at 1"));
}

void testPreloadedComboBoxWidget(TestInvocation* t) {
  t->setName(F("ComboBox with preloaded initial value"));
  t->verify(helper.goToElementById(TestElement::PRELOADED_COMBO_BOX), F("Element not found"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("be"), F("Expected 'be' on initial load"));
  t->verify(MODEL.isSelectable, F("Expected 'be' to be selectable"));
  t->verify(MODEL.cursorMode == ViewModel::CursorMode::NO_CURSOR, F("Expected no cursor"));
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getInteractiveLine(), F("be"));
  t->verify(MODEL.cursorMode != ViewModel::CursorMode::NO_CURSOR, F("Expected cursor active"));
  t->verify(MODEL.cursorPosition == 1, F("Expected cursor at 1 after right")); // no other 'be' completions
  helper.pressAndReleaseLeft();
  t->verifyEqual(MODEL.getInteractiveLine(), F("b"));
  t->verify(MODEL.cursorPosition == 0, F("Expected cursor at 0"));
}

void testWizardWidget_coreFunctionality(TestInvocation* t) {
  t->setName(F("Wizard core functionality"));
  t->verify(helper.goToElementById(TestElement::FULL_WIZARD), F("Element not found"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("1st Step"));
  t->verify(!MODEL.hasPrev, F("Expected hasPrev to be false"));
  t->verify(MODEL.hasNext, F("Expected hasNext to be true"));
  helper.pressAndReleaseLeft();
  t->verifyEqual(MODEL.getTitleLine_P(), F("1st Step")); // no change
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Middle Step"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("two")); // preloaded value
  helper.pressAndReleaseUp();
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("one"));
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Last Step"));
  t->verify(!MODEL.hasNext, F("Expected hasNext to be false"));
  t->verify(MODEL.hasPrev, F("Expected hasPrev to be true"));
  helper.pressAndReleaseRight();
  t->verifyEqual(MODEL.getTitleLine_P(), F("Last Step")); // no change
  helper.pressAndReleaseLeft();
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("one")); // remembers new selection
  helper.pressAndReleaseSelectEnter();
  t->verifyEqual(capturedWizardValues[0], F("shoe"));
  t->verifyEqual(capturedWizardValues[1], F("buckle")); // changed this one
  t->verifyEqual(capturedWizardValues[2], F("buckle"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"));
}

void testWizardWidget_emptyModel(TestInvocation* t) {
  t->setName(F("Wizard with empty model"));
  t->verify(helper.goToElementById(TestElement::EMPTY_WIZARD), F("Element not found"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("1st Step"));
  t->verifyEqual(MODEL.getInteractiveLine_P(), F("one"));
  helper.pressAndReleaseSelectEnter();
  t->verifyEqual(capturedWizardValues[0], F("buckle"));
  t->verify(capturedWizardValues[1] == nullptr, F("Expected nullptr for step 2"));
  t->verify(capturedWizardValues[2] == nullptr, F("Expected nullptr for step 3"));
  t->verifyEqual(MODEL.getTitleLine_P(), F("Main Menu"));
}


void testViewModel_moveConstructorPreservesIsCancelable(TestInvocation* t) {
  t->setName(F("ViewModel move constructor preserves isCancelable"));
  ViewModel a(UIElement::Type::POPUP);
  a.isCancelable = true;
  ViewModel b = static_cast<ViewModel&&>(a);
  t->verify(b.isCancelable, F("isCancelable should survive a move-construct"));
}

void testViewModel_moveAssignmentPreservesIsCancelable(TestInvocation* t) {
  t->setName(F("ViewModel move assignment preserves isCancelable"));
  ViewModel a(UIElement::Type::POPUP);
  a.isCancelable = true;
  ViewModel b(UIElement::Type::UNDEFINED);
  b = static_cast<ViewModel&&>(a);
  t->verify(b.isCancelable, F("isCancelable should survive a move-assignment"));
}

void after() {
  // Return to initial state so memory matches
  helper.reset();
  if (capturedText) free(capturedText);
  if (capturedSelectionName) free(capturedSelectionName);
  if (capturedSelectionValue) free(capturedSelectionValue);
  for (uint8_t i = 0; i <  capturedWizardNumSelections; i++) {
    if (capturedWizardValues[i]) free(capturedWizardValues[i]);    
  }
  if (capturedWizardValues) delete[] capturedWizardValues;
  capturedText = nullptr;
  capturedSelectionName = nullptr;
  capturedSelectionValue = nullptr;
  capturedWizardValues = nullptr;
  capturedWizardNumSelections = 0;
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
    testTextInputWidget_cursor,
    testDefaultComboBoxWidget,
    testPreloadedComboBoxWidget,
    testWizardWidget_coreFunctionality,
    testWizardWidget_emptyModel,
    testRootLevelSelectionPreserved,
    testBackToSubMenu,
    testSubMenuWidget,
    testViewModel_moveConstructorPreservesIsCancelable,
    testViewModel_moveAssignmentPreservesIsCancelable

  };

  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
