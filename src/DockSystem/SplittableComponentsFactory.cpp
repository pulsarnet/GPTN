//
// Created by nmuravev on 3/21/2022.
//

#include "SplittableComponentsFactory.h"
#include "DockToolbar.h"
#include <DockAreaWidget.h>

ads::CDockAreaTitleBar *SplittableComponentsFactory::createDockAreaTitleBar(ads::CDockAreaWidget *dock_area) const  {
    auto title_bar = new DockToolbar(dock_area);
    //title_bar->setVisible(false);
    return title_bar;
}