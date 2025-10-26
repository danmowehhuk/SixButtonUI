#include <Adafruit_LiquidCrystal.h>
#include <Eventuino.h>
#include <SixButtonUI.h>
#include <Arduino.h>

#define UP_BUTTON_PIN    6
#define DOWN_BUTTON_PIN  4
#define LEFT_BUTTON_PIN  3
#define RIGHT_BUTTON_PIN 7
#define MENU_BUTTON_PIN  2
#define ENTER_BUTTON_PIN 5
#define LCD_RS_PIN       16
#define LCD_E_PIN        17
#define LCD_D7_PIN       21
#define LCD_D6_PIN       20
#define LCD_D5_PIN       19
#define LCD_D4_PIN       18
#define DISPLAY_ROWS     2
#define DISPLAY_COLS     16

/*
 * A basic 16x2 LCD display
 */
Adafruit_LiquidCrystal lcd(
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
  Serial.print("Selected: ");
  if (namePmem) Serial.print(reinterpret_cast<const __FlashStringHelper*>(selectionName));
  else Serial.print(selectionName);
  Serial.print(" with value: ");
  if (valuePmem) Serial.println(reinterpret_cast<const __FlashStringHelper*>(selectionValue));
  else Serial.println(selectionValue);
  return state;
}

void initializeTextBox(TextInputModel* model, void* state) {
  model->setInitialValue(F("bar"));
}

void* saveTextInput(const char* value, void* state) {
  Serial.println("onEnter function received: '" + String(value) + "'");
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
    Serial.print("No match for searchPrefix: '");
    Serial.print(searchPrefix);
    Serial.println("'");
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
  Serial.println(F("Got from wizard:"));
  for (uint8_t i = 0; i < numSteps; i++) {
    Serial.print(F("  "));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.println(selectionValues[i]);
  }
}

extern char __heap_start, *__brkval;

int freeMemory() {
  char top;
  if (__brkval == 0) {
      return &top - &__heap_start;
  } else {
      return &top - __brkval;
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

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println(F("SixButtonUI Example Starting..."));
  
  lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);
  
  sixButtonUI = initSixButtonUI();
  evt.addEventSource(sixButtonUI);
  evt.begin();
  
  Serial.println(F("SixButtonUI Example Ready!"));
}

void loop() {
  evt.poll();  
}
