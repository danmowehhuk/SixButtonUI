// Shared pin defaults for SixButtonUI examples.
//
// Pin values below may be supplied by an external project instead of the
// defaults here — e.g. via `-include Pins.h` on the build command line,
// where that header defines OVERRIDE_PINS plus each of the names below.
#ifndef OVERRIDE_PINS
  #ifndef UP_BUTTON_PIN
  #define UP_BUTTON_PIN    6
  #endif
  #ifndef DOWN_BUTTON_PIN
  #define DOWN_BUTTON_PIN  4
  #endif
  #ifndef LEFT_BUTTON_PIN
  #define LEFT_BUTTON_PIN  3
  #endif
  #ifndef RIGHT_BUTTON_PIN
  #define RIGHT_BUTTON_PIN 7
  #endif
  #ifndef MENU_BUTTON_PIN
  #define MENU_BUTTON_PIN  2
  #endif
  #ifndef ENTER_BUTTON_PIN
  #define ENTER_BUTTON_PIN 5
  #endif
  #ifndef LCD_RS_PIN
  #define LCD_RS_PIN       16
  #endif
  #ifndef LCD_E_PIN
  #define LCD_E_PIN        17
  #endif
  #ifndef LCD_D7_PIN
  #define LCD_D7_PIN       21
  #endif
  #ifndef LCD_D6_PIN
  #define LCD_D6_PIN       20
  #endif
  #ifndef LCD_D5_PIN
  #define LCD_D5_PIN       19
  #endif
  #ifndef LCD_D4_PIN
  #define LCD_D4_PIN       18
  #endif
#endif
