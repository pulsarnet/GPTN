//
// Created by darkp on 10.10.2022.
//

#include "ReachabilityTreeItem.h"
#include "AnalysisTreeItem.h"
#include "ModelTreeItem.h"
#include "../ActionTabWidget/WrappedLayoutWidget.h"
#include "../modules/reachability/reachability_window.h"

ReachabilityTreeItem::ReachabilityTreeItem(AnalysisTreeItem *parent)
        : MainTreeItem(parent)
{
    auto modelItem = dynamic_cast<ProjectTreeItem*>(parent->parentItem())->modelItem();
    auto netModelingTab = modelItem->netModelingTab();
    auto reachabilityTree = new ReachabilityWindow(netModelingTab->ctx()->net(), netModelingTab->ctx()->net()->reachability());
    m_reachabilityTab = new WrappedLayoutWidget(reachabilityTree);
}

WrappedLayoutWidget *ReachabilityTreeItem::reachabilityTab() {
    return m_reachabilityTab;
}

QVariant ReachabilityTreeItem::data(int column) const noexcept {
    return "Reachability Tree";
}

QIcon ReachabilityTreeItem::icon() const noexcept {
    return QIcon(":/images/tree.svg");
}
