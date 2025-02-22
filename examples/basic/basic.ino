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
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(viewModel->titleLine);
  lcd.setCursor(0, 1);
  lcd.print(viewModel->interactiveLine);
};

using namespace sixbuttonui;

void loadSelectorModel(SelectorElement::Model* model, void* state) {
  static const char* names[] = { "one", "two" };
  model->optionNames = names;
  static const char* values[] = { "buckle", "shoe" };
  model->optionValues = values;
  model->numOptions = 2;
  model->currValue = "shoe";
}

static const NavigationConfig* NAV_CONFIG = new NavigationConfig(
  subMenu()
      ->withTitle("Main Menu")
      ->withMenuItems(
        subMenu()
          ->withTitle("First"),
        selector()
          ->withTitle("Second")
          ->withModelFunction(loadSelectorModel),
        subMenu()
          ->withTitle("Third")
          ->withMenuItems(
            subMenu()
              ->withTitle("Low"),
            subMenu()
              ->withTitle("Medium"),
            subMenu()
              ->withTitle("High")
          )
      ),
  subMenu()
      ->withTitle("Settings")
      ->withMenuItems(
        subMenu()
          ->withTitle("Clock"),
        subMenu()
          ->withTitle("Date")
      )
);
        

SixButtonUI sixButtonUI(
      UP_BUTTON_PIN, DOWN_BUTTON_PIN, LEFT_BUTTON_PIN,
      RIGHT_BUTTON_PIN, MENU_BUTTON_PIN, ENTER_BUTTON_PIN,
      NAV_CONFIG, renderLCDDisplay
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