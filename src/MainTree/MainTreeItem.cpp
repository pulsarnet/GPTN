//
// Created by darkp on 08.10.2022.
//

#include "MainTreeItem.h"
#include <QIcon>

MainTreeItem::MainTreeItem(MainTreeItem *parent)
    : m_parentItem(parent)
{
    if (parent) {
        // Добавим текущий элемент как потомок parent
        parent->insertChild(parent->childCount(), this);
    }
}

int MainTreeItem::childCount() const noexcept {
    return (int)m_childItems.size();
}

int MainTreeItem::columnCount() const noexcept {
    // По умолчанию одна колонка
    return 1;
}

int MainTreeItem::childNumber() const noexcept  {
    //Если есть родитель - найти свой номер в списке его потомков
    if (m_parentItem) return (int)m_parentItem->m_childItems.indexOf(const_cast<MainTreeItem*>(this));
    return 0; //Иначе вернуть 0
}

bool MainTreeItem::insertChild(int row, MainTreeItem *item) {
    if (!item)
        return false;

    m_childItems.insert(row, item);
    return true;
}

QVariant MainTreeItem::data(int column) const noexcept {
    if (column != 0) return {};
    return QString("Root");
}

QIcon MainTreeItem::icon() const noexcept {
    return {};
}

MainTreeItem::~MainTreeItem() {
    for (auto item : m_childItems) {
        delete item;
    }
}
