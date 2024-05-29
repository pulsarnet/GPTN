#include "SplittableComponentsFactory.h"
#include <DockAreaWidget.h>

using namespace ads;

ads::CDockAreaTitleBar *SplittableComponentsFactory::createDockAreaTitleBar(ads::CDockAreaWidget *dockArea) const  {
    auto titleBar = new CDockAreaTitleBar(dockArea);
    return titleBar;
}