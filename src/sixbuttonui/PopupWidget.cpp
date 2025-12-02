#include "PopupWidget.h"
#include "../SixButtonUI.h"

void* PopupWidget::onEnter(uint8_t value, void* widgetModel, void* state) {
  PopupModel* m = static_cast<PopupModel*>(widgetModel);
  if (_popupElement->getPopupType() == PopupWidget::Type::USER_CANCELABLE
      || _popupElement->getPopupType() == PopupWidget::Type::TIMER_CANCELABLE) {
    m->getController()->dismissPopup();
  }
  return state;
}

void* PopupWidget::onTimerExpired(uint8_t value, void* widgetModel, void* state) {
  return onEnter(value, widgetModel, state);
}