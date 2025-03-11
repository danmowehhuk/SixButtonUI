#ifndef _sixbuttonui_Widget_h
#define _sixbuttonui_Widget_h


#include "UIElement.h"
#include "ViewModel.h"
#include "WidgetModel.h"

class Widget {

  public:
    virtual ~Widget() = 0;

  protected:
    Widget(UIElement* wConf): _wConf(wConf) {};
    const UIElement* _wConf;
    bool _noRefreshModel = false;
    virtual void initModel() = 0;
    virtual void loadModel(void* state) = 0;

    /*
     * onEnter returns either the same state object passed in or
     * a new one to be passed to future onEnter calls
     */
    virtual void* onEnter(uint8_t value, void* widgetModel, void* state) {};

    virtual void onUpPressed(uint8_t value, void* widgetModel) {};
    virtual void onDownPressed(uint8_t value, void* widgetModel) {};
    virtual void onLeftPressed(uint8_t value, void* widgetModel) {};
    virtual void onRightPressed(uint8_t value, void* widgetModel) {};

    friend class SixButtonUI;

  private:
    Widget(Widget &t) = delete;
    virtual WidgetModel* getModel() = 0;
    virtual ViewModel getViewModel() = 0;
    void preLoadModel();
    void populateModel(void* state);

};
inline Widget::~Widget() {};


#endif
