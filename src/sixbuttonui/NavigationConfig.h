#ifndef _sixbuttonui_NavigationConfig_h
#define _sixbuttonui_NavigationConfig_h


#include "UIElement.h"

/*
 * NavigationConfig is the top-level element of the navigation
 */
class NavigationConfig: public UIElementBase<NavigationConfig> {

  public:
    template <typename... Args>
    NavigationConfig(UIElement* topLevelElement, Args... moreTopLevelElements): UIElementBase(UIElement::ROOT) {
      withChildren(topLevelElement, moreTopLevelElements...);
    };

    // Allow moving
    NavigationConfig(NavigationConfig&& other) noexcept : UIElementBase(static_cast<NavigationConfig&&>(other)) {}
    // But not copying
    NavigationConfig(const NavigationConfig&) = delete;
    NavigationConfig& operator=(const NavigationConfig&) = delete;

  private:
    NavigationConfig() = delete;

};


#endif