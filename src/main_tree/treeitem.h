#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QAction>
#include <DockManager.h>
#include "../ffi/rust.h"

class GraphicScene;

class TreeItem : public QObject
{
public:

    explicit TreeItem(ads::CDockManager* manager, TreeItem* parent = nullptr);

    int row() const;
    int childCount() const {
        return m_childItems.count();
    }
    QString data(int column) const;

    void addChild(TreeItem* child);
    void removeChild(TreeItem* child);

    TreeItem* child(int row) const;
    TreeItem* parentItem() const {
        return m_parentItem;
    }

    void setDockWidget(ads::CDockWidget* widget);
    ads::CDockWidget* dockWidget() const;

    virtual QMenu* contextMenu() {
        return nullptr;
    }

    void setName(const QString& name);
    const QString& name() const;

    ~TreeItem();

protected:

    ads::CDockManager* dockManager() const {
        return m_manager;
    }

private:

    QVector<TreeItem*> m_childItems;
    ads::CDockWidget* m_widget;
    ads::CDockManager* m_manager;
    TreeItem* m_parentItem;
    QString m_name;
};

class RootTreeItem : public TreeItem
{
public:
    explicit RootTreeItem(ads::CDockManager* manager, TreeItem* parent = nullptr);

    QMenu* contextMenu() override;

public slots:

    void onNetCreate(bool checked);

private:

    QAction* m_netCreate = nullptr;
};

class NetTreeItem : public TreeItem
{
public:
    explicit NetTreeItem(ads::CDockManager* manager, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;

public slots:

    void onDecompose(bool checked);

private:

    QAction* m_decompose = nullptr;

    GraphicScene* m_scene;
};

class DecomposeItem : public TreeItem {
public:
    explicit DecomposeItem(ffi::SynthesisContext* ctx, ads::CDockManager* manager, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;

public slots:

    void onSynthesis(bool checked);

private:

    QAction* m_synthesis = nullptr;

    ffi::SynthesisContext* m_ctx = nullptr;
};

class PrimitiveSystemItem : public TreeItem {
public:
    explicit PrimitiveSystemItem(ffi::PetriNet* net, ads::CDockManager* manager, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;

private:
    ffi::PetriNet* m_net;
    GraphicScene* m_scene;
};

class LinearBaseFragmentsItem : public TreeItem {
public:
    explicit LinearBaseFragmentsItem(ffi::PetriNet* net, ads::CDockManager* manager, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;
private:
    ffi::PetriNet* m_net;
    GraphicScene* m_scene;
};

class SynthesisItem : public TreeItem {
public:
    explicit SynthesisItem(ads::CDockManager* manager, TreeItem* parent = nullptr);
};

class MatrixItem : public TreeItem {
public:
    explicit MatrixItem(ads::CDockManager* manager, TreeItem* parent = nullptr);
};

class SynthesisProgramsItem : public TreeItem {
public:
    explicit SynthesisProgramsItem(ads::CDockManager* manager, TreeItem* parent = nullptr);
};

#endif // TREEITEM_H
