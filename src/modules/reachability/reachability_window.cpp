//
// Created by darkp on 06.10.2022.
//

#include <DockAreaWidget.h>
#include <QGridLayout>
#include "reachability_view.h"
#include "reachability_tree_scene.h"
#include "reachability_window.h"
#include "../../DockSystem/DockWidget.h"

ReachabilityWindow::ReachabilityWindow(ffi::PetriNet *net, rust::ReachabilityTree *reachability, QWidget *parent)
    : QWidget(parent)
    , m_net(net)
    , m_reachability(reachability)
    , m_manager(new ads::CDockManager(this))
{

    auto scene = new ReachabilityTreeScene(reachability);
    auto view = new ReachabilityView(this);
    view->setScene(scene);

    m_view = new DockWidget("Reachability Tree");
    m_view->setWidget(view);

    auto area = m_manager->addDockWidget(ads::DockWidgetArea::LeftDockWidgetArea, m_view);
    area->setWindowTitle("Дерево достижимости");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);
    setLayout(new QGridLayout(this));
    layout()->addWidget(m_manager);
    layout()->setContentsMargins(0, 0, 0, 0);
}

