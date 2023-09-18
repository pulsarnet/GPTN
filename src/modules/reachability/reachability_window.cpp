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
{
    auto scene = new ReachabilityTreeScene(reachability);
    m_view = new ReachabilityView(this);
    m_view->setScene(scene);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);
}

