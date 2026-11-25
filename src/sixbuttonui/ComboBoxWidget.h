#ifndef _sixbuttonui_ComboBoxWidget_h
#define _sixbuttonui_ComboBoxWidget_h

#include <stdlib.h>
#include <string.h>
#include "SelectorModel.h"
#include "SelectorWidget.h"
#include "Strings.h"
#include "ViewModel.h"

using namespace SixButtonUIStrings;

class ComboBoxWidget: public SelectorWidget {

  public:
    ComboBoxWidget(ComboBoxElement* config): SelectorWidget(config), _config(config) {};
    ~ComboBoxWidget() override {
      if (_prevSelection) free(_prevSelection);
      if (_tmpInteractiveLine) free(_tmpInteractiveLine);
    };

  protected:

    // Initializes the SelectorModel just like a SelectorWidget. However, the ComboBoxWidget
    // needs to reload the model on every render, so set _noRefreshModel to false.
    virtual void initModel() override {
      SelectorWidget::initModel();
      _noRefreshModel = false;
    };

    void loadModel(void* state) override {
      // This method is called on every render, but the model does not need to be reloaded
      // every time; e.g. up/down triggers re-render but not reload.
      if (!_doModelReload) return;

      _model->_currIndex = 0;

      // Call the provided model function to load the options for the current search prefix.
      // It is the user's responsibility to ensure that the options loaded actually begin with
      // the search prefix.
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }

      // Do some additional model setup on the first load.
      if (_isFirstModelLoad) {

        // Initialize the search prefix for subsequent loads.
        if (_model->_initialSearchPrefix) {
          _model->setSearchPrefix(_model->_initialSearchPrefix);
        } else {
          _model->setSearchPrefix("");
        }

        // If a current value is set, preselect the option with that value.
        if (_model->_currValue && _model->selectOptionWithValueRaw(_model->_currValue, _model->_isCurrValuePmem)) {

          // If no initial search prefix is set, use the matching option name as the new search prefix.
          if (!_model->_initialSearchPrefix) {
            char* searchPrefix = dupOptionName();
            _model->setSearchPrefix(searchPrefix);
            free(searchPrefix);
          }
          _isInitialRender = true;
        }
        _isFirstModelLoad = false;
        _doModelReload = true;

      } else if (_prevSelection) {
        // Right or Left changed the search prefix. Stay on what was selected so the selection
        // doesn't jump to the first option, breaking UX continuity
        selectLongestStartOf(_prevSelection);
      }

      // Handle the case where the latest search prefix returned no results.
      if (strlen(_model->getSearchPrefix()) > 0 && _model->getNumOptions() == 0) {
        // Got no results after pressing "right," so trim back to previous search prefix
        onLeftPressed(0, _model);
        
        // Presumably, the previous search prefix got us to this point, so this should
        // not loop more than once
        loadModel(state);
      }

      _cursorPos = _model->_searchPrefix ? strlen(_model->_searchPrefix) : 0;
      _doModelReload = false;
      if (_prevSelection) free(_prevSelection);
      _prevSelection = nullptr;
    };

    /*
     * Update model's searchPrefix trimming off the last character
     */
     void onLeftPressed(uint8_t /*value*/, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      size_t len = strlen(m->_searchPrefix);
      if (len == 0) {
        _isInitialRender = false;
        return;
      }
      if (m->_numOptions > 0) {
        // With no options there's nothing to record, and Right's record, if any, stands
        setPrevSelection();
      }
      _isInitialRender = false;
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
    void onRightPressed(uint8_t /*value*/, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (_isInitialRender) {
        // Starting with a previously selected completion
        _isInitialRender = false;
        _doModelReload = true;
        return;
      }
      if (m->_numOptions == 0) return;
      setPrevSelection();
      if (!_prevSelection) return;
      char newChar = _prevSelection[_cursorPos];

      size_t len = strlen(m->_searchPrefix);
      char* newSearchPrefix = (char*)malloc(len + 2);
      if (newSearchPrefix) {
        strncpy(newSearchPrefix, m->_searchPrefix, len);
        newSearchPrefix[len] = newChar;
        newSearchPrefix[len + 1] = '\0';
        m->setSearchPrefix(newSearchPrefix, true);
        free(newSearchPrefix);
      }
      _doModelReload = true;
    };

    void onLeftLongPressed(uint8_t /*value*/, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->setSearchPrefix("", true); // clear the search prefix
      if (_prevSelection) free(_prevSelection);
      _prevSelection = nullptr;
      _doModelReload = true;
    };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->_numOptions > 0) {
        if (m->getOptionValue() != nullptr && strlen(m->getOptionValue()) > 0) {
          m->_selectionName = dupOptionName();
          m->_isSelectionNamePmem = false;
          m->_isOwnsSelectionName = true;
          return SelectorWidget::onEnter(value, widgetModel, state);
        } else if (m->_currValue) {
          // The option value is empty, but the current value was set. 
          // This happens when the user is explicitly clearing the prior value.
          m->_selectionName = dupOptionName();
          m->_isSelectionNamePmem = false;
          m->_isOwnsSelectionName = true;
          return SelectorWidget::onEnter(value, widgetModel, state);
        }
      }
      return state;
    };

  private:
    ComboBoxElement* _config;
    bool _isFirstModelLoad = true;
    bool _isInitialRender = false;
    uint8_t _cursorPos = 0;
    bool _doModelReload = true;
    // The full name selected before Right or Left changed the search prefix, until the reload
    char* _prevSelection = nullptr;
    char* _tmpInteractiveLine = nullptr;

    void setPrevSelection() {
      if (_prevSelection) free(_prevSelection);
      _prevSelection = dupOptionName();
    };

    // Selects the option whose full name is the longest start of name (name itself, or the
    // prefix of it that a shorter search completes to), or else the first option
    void selectLongestStartOf(const char* name) {
      _model->_currIndex = 0;
      const char* searchPrefix = _model->_searchPrefix ? _model->_searchPrefix : "";
      size_t prefixLen = strlen(searchPrefix);
      if (strncmp(name, searchPrefix, prefixLen) != 0) return;
      const char* rest = name + prefixLen;
      bool found = false;
      size_t foundLen = 0;
      for (uint8_t i = 0; i < _model->_numOptions; i++) {
        const char* option = _model->_optionNames[i];
        bool isPmem = _model->_isOptionNamePmem[i];
        size_t len = isPmem ? strlen_P(option) : strlen(option);
        bool isStart = (isPmem ? strncmp_P(rest, option, len) : strncmp(rest, option, len)) == 0;
        if (isStart && (!found || len > foundLen)) {
          _model->_currIndex = i;
          found = true;
          foundLen = len;
        }
      }
    };

    void setTmpInteractiveLine() {
      if (_tmpInteractiveLine) free(_tmpInteractiveLine);
      _tmpInteractiveLine = dupOptionName();
    };

    char* dupOptionName() {
      return dupOptionName(_model->_currIndex);
    };

    char* dupOptionName(uint8_t i) {
      char* optionName = _model->_isOptionNamePmem[i] ? strdup_P(_model->_optionNames[i])
          : strdup(_model->_optionNames[i]);
      if (!optionName || (_isInitialRender && !_model->_initialSearchPrefix)) {
        // On an initial render the option is the whole name, as getViewModel shows it
        return optionName;
      }
      const char* searchPrefix = _model->_searchPrefix ? _model->_searchPrefix : "";
      size_t prefixLen = strlen(searchPrefix);
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

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::COMBO_BOX, _model);
      if (_config->hasId()) {
        vm.setUIElementId(_config->id);
      }
      if (_model->getNumOptions() > 0) {
        if (_isInitialRender && !_model->_initialSearchPrefix) {
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