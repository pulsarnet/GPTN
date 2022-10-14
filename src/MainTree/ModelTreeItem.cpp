//
// Created by darkp on 08.10.2022.
//

#include "ModelTreeItem.h"

ModelTreeItem::ModelTreeItem(ProjectTreeItem *parent)
    : MainTreeItem(parent)
    , m_modelingTab(new NetModelingTab)
{

}

QVariant ModelTreeItem::data(int column) const noexcept {
    return "Model";
}

QIcon ModelTreeItem::icon() const noexcept {
    return QIcon(":/images/modeling.svg");
}

NetModelingTab *ModelTreeItem::netModelingTab() {
    return m_modelingTab;
}