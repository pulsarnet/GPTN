//
// Created by darkp on 10.10.2022.
//

#include "DecomposeTreeItem.h"
#include "AnalysisTreeItem.h"
#include "ModelTreeItem.h"
#include "../ActionTabWidget/DecomposeModelTab.h"

DecomposeTreeItem::DecomposeTreeItem(AnalysisTreeItem *parent)
    : MainTreeItem(parent)
{
    auto modelItem = dynamic_cast<ModelTreeItem*>(parent->parentItem());
    auto netModelingTab = modelItem->netModelingTab();
    m_decomposeTab = new DecomposeModelTab(netModelingTab);
}

DecomposeModelTab *DecomposeTreeItem::decomposeModelTab() {
    return m_decomposeTab;
}

QVariant DecomposeTreeItem::data(int column) const noexcept {
    return "Decomposition";
}

QIcon DecomposeTreeItem::icon() const noexcept {
    return QIcon(":/images/decompose.svg");
}
