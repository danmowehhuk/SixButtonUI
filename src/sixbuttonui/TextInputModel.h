#ifndef _sixbuttonui_TextInputModel_h
#define _sixbuttonui_TextInputModel_h


#include "WidgetModel.h"

class TextInputModel : public WidgetModel {
  public:
    TextInputModel(const char* initValue, bool pmem = false);
    ~TextInputModel() { clear(); };

    static const uint8_t MAX_LENGTH = 64;

    void initialize(const char* initValue, bool pmem = false);
    void initialize(const __FlashStringHelper* initValue);
    bool isInitialized() { return _isInitialized; };
    void setCursorPos(uint8_t pos);
    uint8_t getCursorPos() { return _cursorPos; };
    void setChar(char c);
    char getChar();
    char* getValue();

    // Disable moving and copying
    TextInputModel(TextInputModel&& other) = delete;
    TextInputModel& operator=(TextInputModel&& other) = delete;
    TextInputModel(const TextInputModel&) = delete;
    TextInputModel& operator=(const TextInputModel&) = delete;

  private:
    bool _isInitialized = false;
    char* _value = nullptr;
    uint8_t _valueLen = 0;
    uint8_t _cursorPos = 0;
    void clear();

};


#endif