#ifndef _sixbuttonui_SubMenuWidget_h
#define _sixbuttonui_SubMenuWidget_h


#include "SelectorWidget.h"

class SubMenuWidget: public SelectorWidget {

  public:
    SubMenuWidget(const SubMenuElement* config): SelectorWidget(config), _config(config) {};
    ~SubMenuWidget() override {};

  protected:
    void initModel() override {
      SelectorWidget::initModel();
      uint8_t count = _config->getChildCount();
      _model->setNumOptions(count);
    }

    void loadModel(void* state) override {
      // Default title is what was set in NavigationConfig
      if (_config->getTitle()) {
        _model->setTitleRaw(_config->getTitle(), _config->isTitlePmem());
      } else {
        _model->setTitle(F(""));
      }
        
      // Call custom model loader function to optionally override the title
      if (_config->modelLoader != 0) {
        _config->modelLoader(_model, state);
      }

      // Load the non-customizable fields from the element configuration
      if (_config->getInstruction()) {
        _model->setInstructionRaw(_config->getInstruction(), _config->isInstructionPmem());
      }
      if (_config->getFooter()) {
        _model->setFooterRaw(_config->getFooter(), _config->isFooterPmem());
      }

      uint8_t count = _config->getChildCount();
      for (uint8_t i = 0; i < count; i++) {
        UIElement* child = _config->getChild(i);

        // Force the UIElement* into a char* but set allocate* to false so it's
        // not strdup'ed
        _model->setOptionRaw(i, child->getTitle(), child->isTitlePmem(), 
        reinterpret_cast<char*>(child), false, false,
         false);
      }

      // Restore last selected if we backed into this submenu
      _model->_currIndex = _config->lastSelected;
    };

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::SUB_MENU, _model);
      if (_model->getNumOptions() > 0) {
        vm.setInteractiveLine(_model->getOptionName(), _model->isOptionNamePmem());
      }
      return vm;
    };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override {
      SelectorModel* m = static_cast<SelectorModel*>(widgetModel);
      if (m->getNumOptions() == 0) return state; // nothing to select
      uint8_t currIdx = m->getCurrIndex();
      _config->lastSelected = currIdx;
      char* uiePtr = m->_optionValues[m->_currIndex];
      UIElement* element = reinterpret_cast<UIElement*>(uiePtr);
      m->getController()->goTo(element);
      return state;
    };

  private:
    SubMenuElement* _config;

};


#endif