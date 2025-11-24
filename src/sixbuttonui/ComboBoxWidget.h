#ifndef _sixbuttonui_ComboBoxWidget_h
#define _sixbuttonui_ComboBoxWidget_h


#include "SelectorModel.h"
#include "SelectorWidget.h"
#include "Strings.h"
#include "ViewModel.h"

using namespace SixButtonUIStrings;

class ComboBoxWidget: public SelectorWidget {

  public:
    ComboBoxWidget(const ComboBoxElement* config): SelectorWidget(config), _config(config) {};
    ~ComboBoxWidget() override {
      if (_prevSearchPrefix) free(_prevSearchPrefix);
      if (_tmpInteractiveLine) free(_tmpInteractiveLine);
    };

  protected:
    void initModel() override {
      SelectorWidget::initModel();
      // model is dynamic, so attempt reload on every render
      _noRefreshModel = false;
    };

    void loadModel(void* state) override {
      if (!_doModelReload) return; // up/down triggers re-render but not reload
      _invalidOption = false;
      _model->_currIndex = 0;
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }

      if (_isFirstModelLoad) {
        _model->setSearchPrefix("", true);
        if (_model->_currValue && _model->selectOptionWithValue(_model->_currValue)) {
          // The matching option name will now be used as the search prefix
          char* searchPrefix = dupOptionName();
          _model->setSearchPrefix(searchPrefix, true);
          free(searchPrefix);
          _isInitialRender = true;
        }
        _isFirstModelLoad = false;
        _doModelReload = true;
      } else if (_prevSearchPrefix && strlen(_prevSearchPrefix) > strlen(_model->getSearchPrefix())) {
        // When moving the cursor left, select the previous search prefix so the selection doesn't
        // jump to the first option, breaking ux continuity
        if (_prevSearchPrefix && strlen(_prevSearchPrefix) > 0) {
          char lastChar[2] = { _prevSearchPrefix[strlen(_prevSearchPrefix) - 1], '\0' };
          _model->selectOptionWithName(lastChar);
        }
      }

      if (strlen(_model->getSearchPrefix()) > 0 && _model->getNumOptions() == 0) {
        // Got no results after pressing "right," so trim back to previous search prefix
        onLeftPressed(0, _model);
        
        // Presumably, the previous search prefix got us to this point, so this should
        // not loop more than once
        loadModel(state);
      }

      _cursorPos = strlen(_model->_searchPrefix);
      validateOptions();
      _doModelReload = false;
    };

    /*
     * Update model's searchPrefix trimming off the last character
     */
     void onLeftPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      _isInitialRender = false;
      size_t len = strlen(m->_searchPrefix);
      if (len == 0) {
        return;
      }
      setPrevSearchPrefix(m->getSearchPrefix());
      char* newSearchPrefix = (char*)malloc(len);
      if (newSearchPrefix) {
        strncpy(newSearchPrefix, m->getSearchPrefix(), len - 1);
        newSearchPrefix[len - 1] = '\0';
        m->setSearchPrefix(newSearchPrefix, true);
        free(newSearchPrefix);
      }
      _doModelReload = true;
    };

    /*
     * Update model's searchPrefix appending the character at the cursor position
     */
    void onRightPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (_isInitialRender) {
        // Starting with a previously selected completion
        _isInitialRender = false;
        _doModelReload = true;
        return;
      }
      char* optionName = dupOptionName(m->_currIndex);
      char newChar = optionName[_cursorPos];

      size_t len = strlen(m->_searchPrefix);
      char* newSearchPrefix = (char*)malloc(len + 2);
      if (newSearchPrefix) {
        strncpy(newSearchPrefix, m->_searchPrefix, len);
        newSearchPrefix[len] = newChar;
        newSearchPrefix[len + 1] = '\0';
        m->setSearchPrefix(newSearchPrefix, true);
        free(newSearchPrefix);
      }
      free(optionName);
      _doModelReload = true;
    };

    void onLeftLongPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->setSearchPrefix("", true); // clear the search prefix
      if (_prevSearchPrefix) free(_prevSearchPrefix);
      _prevSearchPrefix = nullptr;
      _doModelReload = true;
    };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->_numOptions > 0 && m->getOptionValue() != nullptr && strlen(m->getOptionValue()) > 0) {
        m->_selectionName = dupOptionName();
        m->_isSelectionNamePmem = false;
        m->_isOwnsSelectionName = true;
        return SelectorWidget::onEnter(value, widgetModel, state);
      }
      return state;
    };

  private:
    ComboBoxElement* _config;
    bool _isFirstModelLoad = true;
    bool _isInitialRender = false;
    uint8_t _cursorPos = 0;
    bool _doModelReload = true;
    bool _invalidOption = false;
    char* _prevSearchPrefix = nullptr;
    char* _tmpInteractiveLine = nullptr;

    void setPrevSearchPrefix(const char* prefix) {
      if (_prevSearchPrefix) free(_prevSearchPrefix);
      _prevSearchPrefix = strdup(prefix);
    };

    void setTmpInteractiveLine() {
      if (_tmpInteractiveLine) free(_tmpInteractiveLine);
      _tmpInteractiveLine = dupOptionName();
    };

    char* dupOptionName() {
      return dupOptionName(_model->_currIndex);
    };

    char* dupOptionName(uint8_t i) {
      const char* searchPrefix = _model->_searchPrefix ? _model->_searchPrefix : "";
      size_t prefixLen = strlen(searchPrefix);
      char* optionName = _model->_isOptionNamePmem[i] ? strdup_P(_model->_optionNames[i])
          : strdup(_model->_optionNames[i]);
      size_t optionLen = strlen(optionName);
      char* result = (char*)malloc(prefixLen + optionLen + 1);
      if (result) {
        memcpy(result, searchPrefix, prefixLen);
        memcpy(result + prefixLen, optionName, optionLen);
        result[prefixLen + optionLen] = '\0';
      }
      free(optionName);
      return result;
    };

    void validateOptions() {
      for (uint8_t i = 0; i < _model->_numOptions; i++) {
        char* optionName = dupOptionName(i);
        if (!startsWith(optionName, _model->_searchPrefix)) {
          _invalidOption = true;
#if defined (DEBUG)
          Serial.print(F("'"));
          Serial.print(optionName);
          Serial.print(F("' does not start with search prefix: '"));
          Serial.print(_model->_searchPrefix);
          Serial.println(F("'"));
#endif
        }
        free(optionName);
      }
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::COMBO_BOX, _model);
      if (_model->getNumOptions() > 0) {
        if (_invalidOption) {
          // Indicates that one or more options don't start with the search prefix
          vm.setInteractiveLine(PSTR("  err"), true);
        } else {
          if (_isInitialRender) {
            // Option name is initial value - display as-is
            vm.setInteractiveLine(_model->getOptionName(), _model->isOptionNamePmem());
          } else {
            // Prepend option name with search prefix
            setTmpInteractiveLine();
            vm.setInteractiveLine(_tmpInteractiveLine, false);
          }
          vm.cursorPosition = _cursorPos;
          if (_model->getOptionValue() == nullptr) {
            vm.isSelectable = false;
          } else {
            vm.isSelectable = true;
          }  
        }

      if (_isInitialRender) {
          vm.cursorMode = ViewModel::NO_CURSOR;
        } else {
          vm.cursorMode = ViewModel::UNDERLINE;
        }
      }
      return vm;
    };

};


#endif