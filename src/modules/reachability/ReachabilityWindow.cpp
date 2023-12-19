#include <DockAreaWidget.h>
#include <QGridLayout>
#include "ReachabilityView.h"
#include "ReachabilityTreeScene.h"
#include "ReachabilityWindow.h"
#include <ptn/net.h>

// todo: maybe rename to ReachabilityController which generate graph,tree.
ReachabilityWindow::ReachabilityWindow(ptn::net::PetriNet *net, QWidget *parent)
    : QWidget(parent)
    , m_net(net)
{
    auto scene = new ReachabilityTreeScene();
    m_view = new ReachabilityView(this);
    m_view->setScene(scene);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void ReachabilityWindow::reload() {
    // todo delete old
    auto old = m_tree;
    Q_UNUSED(old);

    m_tree = m_net->reachability();
    auto scene = static_cast<ReachabilityTreeScene*>(m_view->scene());
    scene->setTree(m_tree);
}