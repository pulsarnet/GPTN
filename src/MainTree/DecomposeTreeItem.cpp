//
// Created by darkp on 10.10.2022.
//

#include "DecomposeTreeItem.h"
#include "ModelTreeItem.h"
#include "../ActionTabWidget/DecomposeModelTab.h"

DecomposeTreeItem::DecomposeTreeItem(ProjectMetadata *metadata, MainTreeItem *parent)
    : MainTreeItem(parent)
{
    m_decomposeTab = new DecomposeModelTab(metadata);
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