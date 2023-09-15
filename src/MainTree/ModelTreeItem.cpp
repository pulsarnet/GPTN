#include "ModelTreeItem.h"

ModelTreeItem::ModelTreeItem(MainTreeItem *item, NetModelingTab *tab)
    : MainTreeItem(item)
    , m_modelingTab(tab)
{

}

QVariant ModelTreeItem::data(int column) const noexcept {
    Q_UNUSED(column)
    return "Model";
}

QIcon ModelTreeItem::icon() const noexcept {
    return QIcon(":/images/modeling.svg");
}

NetModelingTab *ModelTreeItem::netModelingTab() {
    return m_modelingTab;
}