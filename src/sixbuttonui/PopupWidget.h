#ifndef _sixbuttonui_PopupWidget_h
#define _sixbuttonui_PopupWidget_h

#include "Widget.h"

class PopupWidget: public Widget {

  public:

    enum Type : uint8_t {
      USER_CANCELABLE,
      TIMER_CANCELABLE,
      FATAL
    };

    class PopupElement: public UIElementBase<PopupElement> {

      public:
        PopupElement(): UIElementBase(UIElement::Type::POPUP) {};

        void clear() {
          if (_message && !_isMessagePmem) free(_message);
          _message = nullptr;
          _isMessagePmem = false;
          _popupType = PopupWidget::Type::USER_CANCELABLE;
          _returnToConfig = nullptr;
          _returnToWidget = nullptr;
          _returnToState = nullptr;
        };

        void setPopupType(PopupWidget::Type popupType) {
          _popupType = popupType;
        };

        void setMessage(const char* message, bool pmem = false) {
          if (_message && !_isMessagePmem) free(_message);
          _isMessagePmem = pmem;
          if (_isMessagePmem) {
            _message = message;
          } else {
            strdup(message);
          }
        };

        void setReturnTo(UIElement* returnToConfig, Widget* returnToWidget, void* returnToState) {
          _returnToConfig = returnToConfig;
          _returnToWidget = returnToWidget;
          _returnToState = returnToState;
        };

        const char* getMessage() const { return _message; };
        bool isMessagePmem() const { return _isMessagePmem; };
        PopupWidget::Type getPopupType() const { return _popupType; };
        UIElement* getReturnToConfig() const { return _returnToConfig; };
        Widget* getReturnToWidget() { return _returnToWidget; };
        void* getReturnToState() { return _returnToState; };

        // disable moving and copying
        PopupElement(PopupElement&& other) = delete;
        PopupElement& operator=(PopupElement&& other) = delete;
        PopupElement(const PopupElement&) = delete;
        PopupElement& operator=(const PopupElement&) = delete;

      private:
        char* _message = nullptr;
        bool _isMessagePmem = false;
        PopupWidget::Type _popupType = PopupWidget::Type::USER_CANCELABLE;
        UIElement* _returnToConfig = nullptr;
        Widget* _returnToWidget = nullptr;
        void* _returnToState = nullptr;
    };

    class PopupModel : public WidgetModel {
      public:
        PopupModel() {};
        ~PopupModel() override {};
    };

    PopupWidget(const PopupElement* config): Widget(config) {};
    ~PopupWidget() {
      _popupElement->clear();
      if (_model) delete _model;
    };

    void initModel() override { 
      _model = new PopupModel(); 
    };

    void loadModel(void* state) override {};
    WidgetModel* getModel() override { return _model; };

    void* onEnter(uint8_t value, void* widgetModel, void* state) override;
    void* onTimerExpired(uint8_t value, void* widgetModel, void* state) override;

    ViewModel getViewModel() override {
      ViewModel vm(UIElement::Type::POPUP, _model);
      vm.setTitleLine(_popupElement->getMessage(), _popupElement->isMessagePmem());
      vm.isCancelable = _popupElement->getPopupType() == PopupWidget::Type::USER_CANCELABLE;
      return vm;
    };

    static PopupElement* getPopupElement() {
      return _popupElement;
    };

  private:
    static PopupElement* const _popupElement;
    PopupModel* _model = nullptr;

};
inline PopupWidget::PopupElement* const PopupWidget::_popupElement = new PopupWidget::PopupElement();


#endif