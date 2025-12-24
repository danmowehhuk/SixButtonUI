#ifndef _sixbuttonui_SelectorWidget_h
#define _sixbuttonui_SelectorWidget_h


#include <Arduino.h>
#include "SelectorModel.h"
#include "Widget.h"

class SelectorWidget: public Widget {

  public:
    SelectorWidget(const SelectorElement* config): Widget(config), _config(config) {};
    ~SelectorWidget() override {
      if (_model) delete _model;
    };

  protected:
    SelectorModel* _model = nullptr;

    // Initializes the SelectorModel. The default behavior for a selector is to call the user's
    // model function on the first widget render, but not to reload the model on subsequent renders.
    // Note: All the options will be loaded and will remain in memory until the widget is destroyed.
    // If there are many options, it may be better to use a ComboBoxWidget instead. The ComboBoxWidget
    // reloads the model on every render, so performance is slower, but it never loads more than
    // a fixed number of options.
    virtual void initModel() override {
      _model = new SelectorModel();
      _noRefreshModel = true;
    }

    // Loads the model using the user's model function and sets the initial selection based on the 
    // current value (if set).
    void loadModel(void* state) override {
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }
      _model->selectOptionWithValue(_model->_currValue);
    };

    // Wraps the user's onEnter function to provide some default behaviors, since the enter button may be
    // pressed at any time, whether or not a valid selection has been made, or whether the onEnter
    // function sets the next menu location to display.
    void* onEnter(uint8_t value, void* widgetModel, void* state) override {

      // If the model has no options, nothing can be selected, so return the state unchanged.
      // This is the default behavior when no model function is provided, or no options have been
      // set (as in the case of a SubMenuElement with no children).
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->_numOptions == 0) {
        return state;
      }

      // The option name from the array may not be the actual selection name (e.g. for a 
      // ComboBoxWidget, it's just the completion that follows the search prefix). The
      // subclass may populate _selectionName with the actual selection name. If this is
      // the case, use it instead of the option name from the array.
      const char* selectionName = m->_selectionName;
      bool isNamePmem = m->_isSelectionNamePmem;

      if (!selectionName) {
        // Fall back to the option name from the array.
        selectionName = m->_optionNames[m->_currIndex];
        isNamePmem = m->_isOptionNamePmem[m->_currIndex];
      }

      // Get the value for the currently selected option.
      char* selectionValue = m->_optionValues[m->_currIndex];
      bool isValuePmem = m->_isOptionValuePmem[m->_currIndex];

      if (_config->onEnterFunc != 0) {

        // Apply the default logic for setting the next menu location to display. This must be done
        // before calling the user's onEnter function so it can be overridden.
        m->getController()->setNextDefault();

        if (selectionValue != nullptr && strlen(selectionValue) > 0) {
          // The value is non-empty, so a valid selection has been made.
          // Call the provided onEnter function with the selection name and value
          state = _config->onEnterFunc(selectionName, isNamePmem, selectionValue, isValuePmem, state);

        } else if ((!selectionName || strlen(selectionName) == 0 || strcmp(selectionName, " ") == 0)
              && m->_initialSearchPrefix && strlen(m->_initialSearchPrefix) > 0) {
          // The selection value is null or empty, and the search prefix is empty. For a
          // SelectorWidget or SubMenuWidget, this is a no-op. For a ComboBoxWidget, however, this
          // happens when the user is explicitly clearing the prior value.

          // Having a non-empty initial search prefix indicates that 1) this is a ComboBoxWidget, and
          // 2) there was a prior value that is now being cleared.

          // Call the provided onEnter function with nullptr for the selection name and value.
          // The user-provided onEnter function is expected to handle this case.
          state = _config->onEnterFunc(nullptr, false, nullptr, false, state);

        } else {
          // No valid selection has been made, so return to the same menu location.
          m->getController()->unsetNext();
        }
      }
      return state;
    };

    void onUpPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->prev();
    };

    void onDownPressed(uint8_t value, void* widgetModel) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      m->next();
    };


  private:
    SelectorElement* _config;

    virtual WidgetModel* getModel() override {
      return _model;
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::SELECTOR, _model);
      if (_config->hasId()) {
        vm.setUIElementId(_config->id);
      }
      if (_model->getNumOptions() > 0) {
        vm.setInteractiveLine(_model->getOptionName(), _model->isOptionNamePmem());
        vm.isSelected = _model->isCurrValueSelected();
      }
      return vm;
    };

};


#endif
