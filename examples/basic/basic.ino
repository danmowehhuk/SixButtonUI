#include <Adafruit_LiquidCrystal.h>
#include <Eventuino.h>
#include <SixButtonUI.h>

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

void renderLCDDisplay(ViewModel* viewModel) {
  // Print the contents of the view model in a way
  // that fits the display device
  lcd.noBlink();
  lcd.noCursor();
  lcd.clear();
  lcd.setCursor(0, 0);
  if (viewModel->isTitlePmem()) {
    lcd.print(viewModel->getTitleLine_P());
  } else {
    lcd.print(viewModel->getTitleLine());
  }
  lcd.setCursor(0, 1);
  if (viewModel->isInteractivePmem()) {
    lcd.print(viewModel->getInteractiveLine_P());
  } else {
    lcd.print(viewModel->getInteractiveLine());
  }
  lcd.setCursor(viewModel->cursorPosition, 1);
  if (viewModel->cursorMode != ViewModel::CursorMode::NO_CURSOR) {
    lcd.cursor();    
  }
};

using namespace sixbuttonui;

void loadSelectorModel(SelectorModel* model, void* state) {
  model->setNumOptions(2);
  model->setCurrValue("shoe");
  model->setOption(0, F("one"), F("buckle"));
  model->setOption(1, F("two"), F("shoe"));
}

void initializeTextBox(TextInputModel* model, void* state) {
  model->setInitialValue(F("bar"));
}

void* saveTextInput(const char* value, void* state) {
  Serial.println("onEnter function received: '" + String(value) + "'");
  return state;
}

NavigationConfig* myNavigationConfig() {
  return new NavigationConfig(
  subMenu()
      ->withTitle(F("Main Menu"))
      ->withInstruction(F("Press"))
      ->withFooter(F("Back"))
      ->withMenuItems(
        subMenu()
          ->withTitle(F("First")),
        selector()
          ->withTitle("Second") // leave non-PROGMEM for test
          ->withInstruction("Check")
          ->withFooter("Enter")
          ->withModelFunction(loadSelectorModel),
        textInput()
          ->withTitle(F("Third"))
          ->withInitialValue(F("foo"))
          ->withModelFunction(initializeTextBox)
          ->onEnter(saveTextInput),
        subMenu()
          ->withTitle(F("Fourth"))
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
};

SixButtonUI sixButtonUI(
      UP_BUTTON_PIN, DOWN_BUTTON_PIN, LEFT_BUTTON_PIN,
      RIGHT_BUTTON_PIN, MENU_BUTTON_PIN, ENTER_BUTTON_PIN,
      myNavigationConfig(), renderLCDDisplay
);

Eventuino evt;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  lcd.begin(DISPLAY_COLS, DISPLAY_ROWS);

  evt.addEventSource(&sixButtonUI);
  evt.begin();
}

void loop() {
  evt.poll();  
}