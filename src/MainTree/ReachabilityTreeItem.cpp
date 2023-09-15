//
// Created by darkp on 10.10.2022.
//

#include "ReachabilityTreeItem.h"
#include "ModelTreeItem.h"
#include "../ActionTabWidget/WrappedLayoutWidget.h"
#include "../modules/reachability/reachability_window.h"
#include "../Core/ProjectMetadata.h"
#include "../Core/FFI/rust.h"

ReachabilityTreeItem::ReachabilityTreeItem(ProjectMetadata* metadata, MainTreeItem *parent)
        : MainTreeItem(parent)
{
    auto net = metadata->context()->net();
    auto reachabilityTree = new ReachabilityWindow(net, net->reachability());
    m_reachabilityTab = new WrappedLayoutWidget(reachabilityTree);
}

WrappedLayoutWidget *ReachabilityTreeItem::reachabilityTab() {
    return m_reachabilityTab;
}

QVariant ReachabilityTreeItem::data(int column) const noexcept {
    Q_UNUSED(column)
    return "Reachability Tree";
}

QIcon ReachabilityTreeItem::icon() const noexcept {
    return QIcon(":/images/tree.svg");
}
