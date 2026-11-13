// Bare-metal AVR port of ../test-suite-mcp/test-suite-mcp.ino - same
// shared tests (../test-suite-mcp/McpTests.h), plus a test of poll()
// reading codes from UART0, linked for NO_ARDUINO+HAL_AVR.
#include <BareMetalHAL.h>

#include "../test-suite-mcp/McpTests.h"

// Stands in for the UART0 RX ISR: SimulIDE's CLI can't send serial input.
void injectRx(const char* s) {
  using namespace BareMetalHAL::Uart0::rxDetail;
  for (; *s; s++) {
    buffer[tail] = *s;
    tail = (tail + 1) & 63;
  }
}

void testPoll_readsCodesFromUart(TestInvocation* t) {
  t->setName(F("poll() reads and dispatches codes received on UART0"));
  t->verify(helper.goToElementById(TestElement::ITEM_SELECTOR), F("Element not found"));
  t->verifyEqual(MODEL.getInteractiveLine(), "alpha");
  injectRx("debug noise\n6BUI: DN\n");
  sixButtonUI->poll();
  t->verifyEqual(MODEL.getInteractiveLine(), "beta");
  t->verify(BareMetalHAL::Uart0::available() == 0, F("poll() should drain the RX buffer"));
}

int main() {
  BareMetalHAL::Uart0::begin(9600);
  BareMetalHAL::Uart0::enableRx();
  BareMetalHAL::timingInit();

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
    testDispatch_menuBackTogglesRootMenu,
    testPoll_readsCodesFromUart
  };

  runTestSuiteShowMem(tests, nullptr, after);
  while (true) {}
}
