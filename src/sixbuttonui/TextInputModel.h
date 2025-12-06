#ifndef _sixbuttonui_TextInputModel_h
#define _sixbuttonui_TextInputModel_h


#include "WidgetModel.h"

class TextInputModel : public WidgetModel {
  public:
    TextInputModel(const char* initValue, bool pmem = false);
    ~TextInputModel() { clear(); };

    static const uint8_t MAX_LENGTH = 64;

    bool isInitialized() { return _isInitialized; };
    void setInitialValue(const char* initValue); // always allocates
    void setInitialValue(const __FlashStringHelper* initValue); // always allocates
    void setInitialValueRaw(const char* initValue, bool pmem);
    uint8_t getCursorPos() { return _cursorPos; };
    char* getValue();

    // Disable moving and copying
    TextInputModel(TextInputModel&& other) = delete;
    TextInputModel& operator=(TextInputModel&& other) = delete;
    TextInputModel(const TextInputModel&) = delete;
    TextInputModel& operator=(const TextInputModel&) = delete;

    static char getNext(char c);
    static char getPrev(char c);

  private:
    bool _isInitialized = false;
    char* _value = nullptr;
    uint8_t _valueLen = 0;
    uint8_t _cursorPos = 0;
    void setCursorPos(uint8_t pos);
    void setChar(char c);
    char getChar();
    void clear();

    friend class TextInputWidget;
};


#endif