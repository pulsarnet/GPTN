//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_MAINTREEMODEL_H
#define FFI_RUST_MAINTREEMODEL_H

#include <QAbstractItemModel>

class MainTreeItem;

class MainTreeModel : public QAbstractItemModel {

    Q_OBJECT

public:

    MainTreeModel();

    MainTreeItem* getItem(const QModelIndex& parent) const;
    bool addChild(MainTreeItem* child, const QModelIndex &parent = QModelIndex());
    QModelIndex indexForTreeItem(MainTreeItem*) const;

    /// Overrides
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;

    QModelIndex parent(const QModelIndex &child) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QVariant data(const QModelIndex &index, int role) const override;

private:

    MainTreeItem* m_root;

};


#endif //FFI_RUST_MAINTREEMODEL_H
