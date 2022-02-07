#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <DockManager.h>
class TreeItem;

class TreeModel : public QAbstractItemModel
{
public:
    explicit TreeModel(ads::CDockManager* dockManager, QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                          const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    TreeItem* getItem(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    TreeItem* root() const {
        return rootItem;
    }

    QModelIndex indexForTreeItem(TreeItem* item);

    void emitBeginInsertRows(const QModelIndex& index, int first, int last) {
        beginInsertRows(index, first, last);
    }

    void emitEndInsertRows() {
        endInsertRows();
    }

    ~TreeModel();

    ads::CDockManager* dockManager() const {
        return m_dockManager;
    }

private:

    TreeItem *rootItem;
    ads::CDockManager* m_dockManager;
};

#endif // TREEMODEL_H
