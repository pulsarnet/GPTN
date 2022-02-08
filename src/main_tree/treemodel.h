#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <DockManager.h>
class TreeItem;

class TreeModel : public QAbstractItemModel
{
public:
    explicit TreeModel(ads::CDockManager* dockManager, QObject *parent = nullptr);

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;
    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    [[nodiscard]] QModelIndex parent(const QModelIndex &index) const override;
    [[nodiscard]] TreeItem* getItem(const QModelIndex &index) const;
    [[nodiscard]] int rowCount(const QModelIndex &parent) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;
    bool removeRows(int row, int count, const QModelIndex &parent) override;

    [[nodiscard]] TreeItem* root() const {
        return rootItem;
    }

    QModelIndex indexForTreeItem(TreeItem* item);

    void emitBeginInsertRows(const QModelIndex& index, int first, int last) {
        beginInsertRows(index, first, last);
    }

    void emitEndInsertRows() {
        endInsertRows();
    }

    ~TreeModel() override;

    [[nodiscard]] ads::CDockManager* dockManager() const {
        return m_dockManager;
    }

private:

    TreeItem *rootItem;
    ads::CDockManager* m_dockManager;
};

#endif // TREEMODEL_H
