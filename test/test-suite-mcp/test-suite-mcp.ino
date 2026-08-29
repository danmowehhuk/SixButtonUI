#include <TestTool.h>
#include <SixButtonUI.h>
#include <mcp/Mcp.h>

void testParser_validCodes(TestInvocation* t) {
  t->setName(F("LineParser recognizes all 12 valid codes"));
  struct { const char* line; Mcp::Code expected; } cases[] = {
    {"6BUI: UP\n", Mcp::Code::UP},
    {"6BUI: DN\n", Mcp::Code::DN},
    {"6BUI: LF\n", Mcp::Code::LF},
    {"6BUI: RT\n", Mcp::Code::RT},
    {"6BUI: EN\n", Mcp::Code::EN},
    {"6BUI: ME\n", Mcp::Code::ME},
    {"6BUI: UP-L\n", Mcp::Code::UP_L},
    {"6BUI: DN-L\n", Mcp::Code::DN_L},
    {"6BUI: LF-L\n", Mcp::Code::LF_L},
    {"6BUI: RT-L\n", Mcp::Code::RT_L},
    {"6BUI: EN-L\n", Mcp::Code::EN_L},
    {"6BUI: ME-L\n", Mcp::Code::ME_L},
  };
  for (uint8_t i = 0; i < 12; i++) {
    Mcp::LineParser parser;
    Mcp::Code result = Mcp::Code::NONE;
    for (const char* p = cases[i].line; *p; p++) {
      result = parser.feed(*p);
    }
    t->verify(result == cases[i].expected, F("Unexpected code for a valid line"));
  }
}

void testParser_ignoresGarbagePrefix(TestInvocation* t) {
  t->setName(F("LineParser ignores lines without the 6BUI prefix"));
  Mcp::LineParser parser;
  Mcp::Code result = Mcp::Code::NONE;
  const char* line = "some other debug output\n";
  for (const char* p = line; *p; p++) {
    result = parser.feed(*p);
  }
  t->verify(result == Mcp::Code::NONE, F("Garbage line should not produce a code"));
}

void testParser_ignoresUnknownCode(TestInvocation* t) {
  t->setName(F("LineParser ignores a well-formed but unknown code"));
  Mcp::LineParser parser;
  Mcp::Code result = Mcp::Code::NONE;
  const char* line = "6BUI: XX\n";
  for (const char* p = line; *p; p++) {
    result = parser.feed(*p);
  }
  t->verify(result == Mcp::Code::NONE, F("Unknown code should not produce a match"));
}

void testParser_discardsOverlongLine(TestInvocation* t) {
  t->setName(F("LineParser discards a line longer than its buffer"));
  Mcp::LineParser parser;
  Mcp::Code result = Mcp::Code::NONE;
  // 40 'X' characters, well past BUFFER_SIZE (24), then a valid code.
  for (uint8_t i = 0; i < 40; i++) {
    result = parser.feed('X');
  }
  result = parser.feed('\n');
  t->verify(result == Mcp::Code::NONE, F("Overlong line should be discarded, not misparsed"));

  // Parser must resync correctly on the next line.
  const char* line = "6BUI: UP\n";
  for (const char* p = line; *p; p++) {
    result = parser.feed(*p);
  }
  t->verify(result == Mcp::Code::UP, F("Parser should recover on the next line after an overlong one"));
}

void testParser_splitAcrossFeedCalls(TestInvocation* t) {
  t->setName(F("LineParser recognizes a code fed one byte at a time across calls"));
  Mcp::LineParser parser;
  t->verify(parser.feed('6') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed('B') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed('U') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed('I') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed(':') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed(' ') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed('E') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed('N') == Mcp::Code::NONE, F("partial"));
  t->verify(parser.feed('\n') == Mcp::Code::EN, F("Full code should resolve on the newline"));
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  TestFunction tests[] = {
    testParser_validCodes,
    testParser_ignoresGarbagePrefix,
    testParser_ignoresUnknownCode,
    testParser_discardsOverlongLine,
    testParser_splitAcrossFeedCalls
  };

  runTestSuiteShowMem(tests);
}

void loop() {}
