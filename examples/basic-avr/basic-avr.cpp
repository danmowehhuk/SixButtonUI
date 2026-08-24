// NO_ARDUINO+HAL_AVR port of ../basic/basic.ino - same UI, driven by
// HD44780 instead of Adafruit_LiquidCrystal.
#include <BareMetalHAL.h>
#include <Eventuino.h>
#include <HD44780.h>
#include <SixButtonUI.h>

using namespace BareMetalHAL;

constexpr uint8_t UP_BUTTON_PIN    = pin(Port::A, 0);
constexpr uint8_t DOWN_BUTTON_PIN  = pin(Port::A, 1);
constexpr uint8_t LEFT_BUTTON_PIN  = pin(Port::A, 2);
constexpr uint8_t RIGHT_BUTTON_PIN = pin(Port::A, 3);
constexpr uint8_t MENU_BUTTON_PIN  = pin(Port::A, 4);
constexpr uint8_t ENTER_BUTTON_PIN = pin(Port::A, 5);

constexpr uint8_t LCD_RS_PIN = pin(Port::C, 7);
constexpr uint8_t LCD_E_PIN  = pin(Port::C, 6);
constexpr uint8_t LCD_D4_PIN = pin(Port::C, 4);
constexpr uint8_t LCD_D5_PIN = pin(Port::C, 2);
constexpr uint8_t LCD_D6_PIN = pin(Port::C, 0);
constexpr uint8_t LCD_D7_PIN = pin(Port::G, 2);
constexpr uint8_t DISPLAY_ROWS = 2;
constexpr uint8_t DISPLAY_COLS = 16;

/*
 * A basic 16x2 LCD display
 */
HD44780 lcd(
      LCD_RS_PIN, LCD_E_PIN, LCD_D4_PIN,
      LCD_D5_PIN, LCD_D6_PIN, LCD_D7_PIN
);

void renderLCDDisplay(ViewModel viewModel) {
  // Print the contents of the view model in a way
  // that fits the display device
  lcd.noBlink();
  lcd.noCursor();
  lcd.clear();
  lcd.setCursor(0, 0);
  if (viewModel.getType() == UIElement::WIZARD) {
    if (viewModel.hasPrev) {
      lcd.print("< ");
    } else {
      lcd.print("  ");
    }
    lcd.setCursor(2, 0);
  }
  if (viewModel.isTitlePmem()) {
    lcd.print(viewModel.getTitleLine_P());
  } else {
    lcd.print(viewModel.getTitleLine());
  }
  if (viewModel.getType() == UIElement::WIZARD) {
    lcd.setCursor(14, 0);
    if (viewModel.hasNext) {
      lcd.print(" >");
    } else {
      lcd.print("  ");
    }
  }
  lcd.setCursor(0, 1);
  if (viewModel.isInteractivePmem()) {
    lcd.print(viewModel.getInteractiveLine_P());
  } else {
    lcd.print(viewModel.getInteractiveLine());
  }
  lcd.setCursor(viewModel.cursorPosition, 1);
  if (viewModel.cursorMode != ViewModel::CursorMode::NO_CURSOR) {
    if (viewModel.isSelectable) {
      lcd.blink();       // Block (blinking) cursor
      lcd.noCursor();
    } else if (viewModel.cursorMode == ViewModel::CursorMode::UNDERLINE) {
      lcd.cursor();      // Underline cursor
      lcd.noBlink();
    }
  }
};

using namespace sixbuttonui;

void loadSelectorModel(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, F("one"), F("buckle"));
  model->setOption(1, F("two"), F("shoe"));
}

void loadSelectorModelRAM(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, "one", "buckle");
  model->setOption(1, "two", "shoe");
}

void* selectorOnEnter(const char* selectionName, bool namePmem, const char* selectionValue, bool valuePmem, void* state) {
  Uart0::print("Selected: ");
  if (namePmem) Uart0::print(reinterpret_cast<const FlashStr*>(selectionName));
  else Uart0::print(selectionName);
  Uart0::print(" with value: ");
  if (valuePmem) Uart0::println(reinterpret_cast<const FlashStr*>(selectionValue));
  else Uart0::println(selectionValue);
  return state;
}

void initializeTextBox(TextInputModel* model, void* state) {
  model->setInitialValue(F("bar"));
}

void* saveTextInput(const char* value, void* state) {
  Uart0::print("onEnter function received: '");
  Uart0::print(value);
  Uart0::println("'");
  return state;
}

void loadComboBox(SelectorModel* model, void* state) {
  char* searchPrefix = model->getSearchPrefix();
  if (!searchPrefix || strlen(searchPrefix) == 0) {
    model->setNumOptions(3);
    model->setOptionRaw(0, "a", false, "a", false);
    model->setOptionRaw(1, "b", false, nullptr, false);
    model->setOptionRaw(2, "c", false, nullptr, false);
  } else if (strcmp(searchPrefix, "c") == 0) {
    model->setNumOptions(3);
    model->setOptionRaw(0, "a", false, nullptr, false);
    model->setOptionRaw(1, "herry", false, "cherry", false);
    model->setOptionRaw(2, "o", false, nullptr, false);
  } else if (strcmp(searchPrefix, "ch") == 0) {
    model->setNumOptions(3);
    model->setOptionRaw(0, "a", false, nullptr, false);
    model->setOptionRaw(1, "e", false, nullptr, false);
    model->setOptionRaw(2, "r", false, nullptr, false);
  } else if (strcmp(searchPrefix, "co") == 0) {
    model->setNumOptions(3);
    model->setOptionRaw(0, "a", false, nullptr, false);
    model->setOptionRaw(1, "b", false, "cob", false);
    model->setOptionRaw(2, "w", false, "cow", false);
  } else if (strcmp(searchPrefix, "cob") == 0) {
    model->setNumOptions(3);
    model->setOptionRaw(0, "a", false, nullptr, false);
    model->setOptionRaw(1, "b", false, nullptr, false);
    model->setOptionRaw(2, "r", false, nullptr, false);
  } else {
    Uart0::print("No match for searchPrefix: '");
    Uart0::print(searchPrefix);
    Uart0::println("'");
    model->setNumOptions(0);
  }
}

void loadComboBoxWithInitialValue(SelectorModel* model, void* state) {
  if (model->getSearchPrefix() == nullptr) { // first load
    model->setCurrValue("cob");
    model->setNumOptions(1);
    model->setOptionRaw(0, "cob", false, "cob", false);
  } else {
    loadComboBox(model, state);
  }
}

void loadComboBoxEmpty(SelectorModel* model, void* state) {
  model->setNumOptions(0);
}

void loadWizardModel(WizardModel* model, void* state) {
  model->setStepInitialValue(0, F("shoe"));
  model->setStepInitialValue(1, "shoe");
  model->setStepInitialValue(2, F("buckle"));
}

void captureWizardValues(char** selectionValues, uint8_t numSteps, void* state) {
  Uart0::println(F("Got from wizard:"));
  for (uint8_t i = 0; i < numSteps; i++) {
    Uart0::print(F("  "));
    Uart0::print((int)i);
    Uart0::print(F(": "));
    Uart0::println(selectionValues[i]);
  }
}

SixButtonUI* initSixButtonUI() {
  NavigationConfig* navConfig = new NavigationConfig(
    subMenu()
      ->withTitle(F("Main Menu"))
      ->withInstruction(F("Press"))
      ->withFooter(F("Back"))
      ->withMenuItems(
        subMenu()
          ->withTitle(F("First"))
          ->withMenuItems(
            selector()
              ->withTitle("RAM-selector") // non-PROGMEM for test
              ->withInstruction("Press")
              ->withFooter("Enter")
              ->withModelFunction(loadSelectorModel)
              ->onEnter(selectorOnEnter),
            selector()
              ->withTitle(F("PMEM-selector")) // PROGMEM for test
              ->withInstruction(F("Drink"))
              ->withFooter(F("Back"))
              ->withModelFunction(loadSelectorModel)
              ->onEnter(selectorOnEnter),
            textInput()
              ->withTitle(F("TextBox"))
              ->withInitialValue(F("foo")) // model function overrides this
              ->withModelFunction(initializeTextBox)
              ->onEnter(saveTextInput),
            comboBox()
              ->withTitle(F("ComboBox1"))
              ->withModelFunction(loadComboBox)
              ->onEnter(selectorOnEnter),
            comboBox()
              ->withTitle(F("ComboBox2"))
              ->withModelFunction(loadComboBoxWithInitialValue)
              ->onEnter(selectorOnEnter),
            comboBox()
              ->withTitle(F("ComboBox3"))
              ->withModelFunction(loadComboBoxEmpty)
              ->onEnter(selectorOnEnter)
          ),
        selector()
          ->withTitle(F("Second"))
          ->withInstruction(F("Check"))
          ->withFooter(F("Enter"))
          ->withModelFunction(loadSelectorModel)
          ->onEnter(selectorOnEnter),
        selector()
          ->withTitle(F("Third"))
          ->withInstruction(F("Check"))
          ->withFooter(F("Enter"))
          ->withModelFunction(loadSelectorModelRAM)
          ->onEnter(selectorOnEnter),
        wizard()
          ->withTitle(F("SetupWizard"))
          ->withInstruction(F("Setup"))
          ->withFooter(F("Enter"))
          ->withModelFunction(loadWizardModel)
          ->withSteps(
            selector()
              ->withTitle(F("1st Step"))
              ->withModelFunction(loadSelectorModel),
            selector()
              ->withTitle(F("Middle Step"))
              ->withModelFunction(loadSelectorModel),
            selector()
              ->withTitle(F("Last Step"))
              ->withModelFunction(loadSelectorModel)
          )
          ->onEnter(captureWizardValues)
        ),
    subMenu()
      ->withTitle(F("Settings"))
      ->withMenuItems(
        subMenu()
          ->withTitle(F("Clock")),
        subMenu()
          ->withTitle(F("Date"))
      )
  );

  SixButtonUI* sixButtonUI = new SixButtonUI(
        UP_BUTTON_PIN,    DOWN_BUTTON_PIN,
        LEFT_BUTTON_PIN,  RIGHT_BUTTON_PIN,
        MENU_BUTTON_PIN,  ENTER_BUTTON_PIN,
        renderLCDDisplay,
        navConfig
  );
  return sixButtonUI;
}

Eventuino evt;
SixButtonUI* sixButtonUI = nullptr;

int main() {
  Uart0::begin(9600);
  timingInit();

  Uart0::println(F("SixButtonUI Example Starting..."));

  lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);

  sixButtonUI = initSixButtonUI();
  evt.addEventSource(sixButtonUI);
  evt.begin();

  Uart0::println(F("SixButtonUI Example Ready!"));

  while (true) {
    evt.poll();
  }
  return 0;
}
