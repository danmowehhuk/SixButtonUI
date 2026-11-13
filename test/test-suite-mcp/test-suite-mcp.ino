#include "McpTests.h"

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testParser_validCodes,
    testParser_ignoresGarbagePrefix,
    testParser_ignoresUnknownCode,
    testParser_discardsOverlongLine,
    testParser_splitAcrossFeedCalls,
    testEscape_plainTextUnchanged,
    testEscape_escapesSpecialChars,
    testEscape_pmemSource,
    testEscape_nullSourceProducesEmptyString,
    testEscape_truncatesCleanlyWhenTooLong,
    testDispatch_upDownMoveSelection,
    testDispatch_leftRightAreNoOpsOnSelector,
    testDispatch_enterCapturesSelectionAndNavigatesBack,
    testDispatch_longPressCodesAlsoFireOnPressed,
    testDispatch_enLongVariantBehavesLikeEnToday,
    testDispatch_menuBackTogglesRootMenu
  };

  runTestSuiteShowMem(tests, nullptr, after);
}

void loop() {}
