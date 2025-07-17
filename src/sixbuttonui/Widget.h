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

    /*
     * By default, the model is refreshed on every render. Set this to true
     * if you want to load the model only when the widget is initially loaded.
     */
    bool _noRefreshModel = false;

    /*
     * initModel is called when the widget is created. It should create the
     * model object and set it to the _model member variable.
     */
    virtual void initModel() = 0;

    /*
     * loadModel is called every time the widget is rendered (unless
     * _noRefreshModel is set to true). It should load the model with the
     * current state.
     */
    virtual void loadModel(void* state) = 0;

    /*
     * onEnter returns either the same state object passed in or
     * a new one to be passed to future onEnter calls
     */
    virtual void* onEnter(uint8_t value, void* widgetModel, void* state) {};

    /*
     * Default implementations of the button handlers.
     */
    virtual void onUpPressed(uint8_t value, void* widgetModel) {};
    virtual void onUpLongPressed(uint8_t value, void* widgetModel) {};
    virtual void onDownPressed(uint8_t value, void* widgetModel) {};
    virtual void onDownLongPressed(uint8_t value, void* widgetModel) {};
    virtual void onLeftPressed(uint8_t value, void* widgetModel) {};
    virtual void onLeftLongPressed(uint8_t value, void* widgetModel) {};
    virtual void onRightPressed(uint8_t value, void* widgetModel) {};
    virtual void onRightLongPressed(uint8_t value, void* widgetModel) {};
    virtual bool onUpLongPressRepeat() { return false; };
    virtual bool onDownLongPressRepeat() { return false; };
    virtual bool onLeftLongPressRepeat() { return false; };
    virtual bool onRightLongPressRepeat() { return false; };

    friend class SixButtonUI;

  private:
    Widget(Widget &t) = delete;
    virtual WidgetModel* getModel() = 0;
    virtual ViewModel getViewModel() = 0;
    void preLoadModel();
    void populateModel(void* state);
    bool _modelLoaded = false;

};
inline Widget::~Widget() {};


#endif
