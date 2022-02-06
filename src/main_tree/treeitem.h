#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVector>
#include <QAction>
#include <DockManager.h>

class GraphicScene;
class TreeModel;

namespace ffi {
    struct SynthesisContext;
    struct DecomposeContext;
    struct PetriNet;
    struct CMatrix;
}

class TreeItem : public QObject
{
public:

    explicit TreeItem(TreeModel* model, TreeItem* parent = nullptr);

    int row() const;
    int childCount() const {
        return m_childItems.length();
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

    void beginInsertRows();
    void endInsertRows();

    void setName(const QString& name);
    const QString& name() const;

    ~TreeItem();

protected:

    ads::CDockManager* dockManager() const;
    TreeModel* model() const;

private:

    QVector<TreeItem*> m_childItems;
    ads::CDockWidget* m_widget;
    TreeModel* m_model;
    TreeItem* m_parentItem;
    QString m_name;
};

class RootTreeItem : public TreeItem
{
public:
    explicit RootTreeItem(TreeModel* model, TreeItem* parent = nullptr);

    QMenu* contextMenu() override;

public slots:

    void onNetCreate(bool checked);

private:

    QAction* m_netCreate = nullptr;
};

class NetTreeItem : public TreeItem
{
public:
    explicit NetTreeItem(TreeModel* model, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;

public slots:

    void onDecompose(bool checked);

private:

    QAction* m_decompose = nullptr;

    GraphicScene* m_scene;
};

class DecomposeItem : public TreeItem {
public:
    explicit DecomposeItem(ffi::DecomposeContext* ctx, TreeModel* model, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;

public slots:

    void onSynthesis(bool checked);

private:

    QAction* m_synthesis = nullptr;

    ffi::DecomposeContext* m_ctx = nullptr;
};

class PrimitiveSystemItem : public TreeItem {
public:
    explicit PrimitiveSystemItem(ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);
private:
    ffi::PetriNet* m_net;
    GraphicScene* m_scene;
};

class LinearBaseFragmentsItem : public TreeItem {
public:
    explicit LinearBaseFragmentsItem(ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);
private:
    ffi::PetriNet* m_net;
    GraphicScene* m_scene;
};

class SynthesisItem : public TreeItem {
public:
    explicit SynthesisItem(ffi::SynthesisContext* ctx, TreeModel* model, TreeItem* parent = nullptr);

private:
    ffi::SynthesisContext* m_ctx = nullptr;
};

class MatrixItem : public TreeItem {
public:
    explicit MatrixItem(ffi::CMatrix* matrix, TreeModel* model, TreeItem* parent = nullptr);
};

class SynthesisProgramsItem : public TreeItem {
public:
    explicit SynthesisProgramsItem(ffi::SynthesisContext* ctx, TreeModel* model, TreeItem* parent = nullptr);

public slots:

    void onProgramSynthesed(ffi::PetriNet* net);
};

class SynthesisProgramItem : public TreeItem {
public:
    explicit SynthesisProgramItem(ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);

private:

    GraphicScene* m_scene;
    ffi::PetriNet* m_net;
};

#endif // TREEITEM_H
