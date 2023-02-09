//
// Created by darkp on 08.10.2022.
//

#include "MainTreeItem.h"
#include "MainTreeModel.h"
#include <QIcon>

MainTreeModel::MainTreeModel()
    : m_root(new MainTreeItem)
{

}

MainTreeItem *MainTreeModel::getItem(const QModelIndex &index) const {
    if (index.isValid()) {
        auto item = static_cast<MainTreeItem*>(index.internalPointer());
        if (item) return item;
    }
    return m_root;
}

bool MainTreeModel::addChild(MainTreeItem* child, const QModelIndex &parent) {
    auto parentItem = getItem(parent);
    int row = parentItem->childCount();
    bool success;
    beginInsertRows(parent, row, row);
    success = parentItem->insertChild(row, child);
    endInsertRows();
    return success;
}

QModelIndex MainTreeModel::indexForTreeItem(MainTreeItem *item) const {
    return createIndex(item->childCount(), 0, item);
}

QModelIndex MainTreeModel::index(int row, int column, const QModelIndex &parent) const {
    if (!hasIndex(row, column, parent)) return {};

    auto parentItem = getItem(parent);
    auto childItem = parentItem->childItem(row);

    return childItem ?
           createIndex(row, column, childItem) : QModelIndex();
}

QModelIndex MainTreeModel::parent(const QModelIndex &child) const {
    if (!child.isValid()) return {};

    auto childItem = getItem(child);
    auto parentItem = childItem->parentItem();

    return parentItem ?
           createIndex(parentItem->childNumber(), 0, parentItem) : QModelIndex();
}

int MainTreeModel::rowCount(const QModelIndex &parent) const {
    return getItem(parent)->childCount();
}

int MainTreeModel::columnCount(const QModelIndex &parent) const {
    return 1;
}

QVariant MainTreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) return m_root->data(section);
    return QVariant();
}

QVariant MainTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return {};

    auto item = getItem(index);
    if (role == Qt::DisplayRole) {
        return item->data(index.column());
    } else if (role == Qt::DecorationRole) {
        return item->icon();
    } else if (role == Qt::SizeHintRole) {
        return QSize(0, 25);
    }

    return {};
}