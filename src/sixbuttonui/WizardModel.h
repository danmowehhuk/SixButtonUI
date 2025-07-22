#ifndef _sixbuttonui_WizardModel_h
#define _sixbuttonui_WizardModel_h


#include "WidgetModel.h"

class WizardModel : public WidgetModel {
  public:
    WizardModel() {};
    ~WizardModel() { clear(); };

    // Disable moving and copying
    WizardModel(WizardModel&& other) = delete;
    WizardModel& operator=(WizardModel&& other) = delete;
    WizardModel(const WizardModel&) = delete;
    WizardModel& operator=(const WizardModel&) = delete;

  private:
    void clear() {};

    friend class WizardWidget;

};


#endif
