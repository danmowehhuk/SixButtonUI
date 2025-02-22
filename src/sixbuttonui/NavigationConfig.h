#ifndef _sixbuttonui_NavigationConfig_h
#define _sixbuttonui_NavigationConfig_h


#include "UIElement.h"

/*
 * NavigationConfig is the top-level element of the navigation
 */
class NavigationConfig: public UIElementBase<NavigationConfig> {

  public:
    template <typename... Args>
    NavigationConfig(UIElement* topLevelElement, Args... moreTopLevelElements): UIElementBase(ROOT) {
      withChildren(topLevelElement, moreTopLevelElements...);
    };

  private:
    NavigationConfig() = delete;
    NavigationConfig(NavigationConfig &t) = delete;

};


#endif