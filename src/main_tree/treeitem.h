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

    enum ObjectType {
        O_Root = 1,
        O_Net,
        O_Decompose,
        O_PrimitiveSystem,
        O_LinearBaseFragments,
        O_Synthesis,
        O_Matrix,
        O_SynthesisPrograms,
        O_SynthesisProgram,
    };

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

    virtual ObjectType item_type() const = 0;
    virtual QVariant toVariant() const {
        return QVariant();
    }

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

    ObjectType item_type() const override {
        return ObjectType::O_Root;
    }

    QVariant toVariant() const override;
    void fromVariant(const QVariant& data);

public slots:

    void onNetCreate(bool checked);

private:

    QAction* m_netCreate = nullptr;
};

class NetTreeItem : public TreeItem
{
public:
    explicit NetTreeItem(TreeModel* model, TreeItem* parent = nullptr);
    explicit NetTreeItem(const QVariant& data, TreeModel* model, TreeItem* parent = nullptr);

    QMenu *contextMenu() override;

    ObjectType item_type() const override {
        return ObjectType::O_Net;
    }

    ffi::PetriNet* net() const;

    QVariant toVariant() const override;

public slots:

    void onDecompose(bool checked);

private:

    void initialize();

private:

    QAction* m_decompose = nullptr;

    GraphicScene* m_scene;
};

class DecomposeItem : public TreeItem {
public:
    explicit DecomposeItem(ffi::DecomposeContext* ctx, TreeModel* model, TreeItem* parent);
    explicit DecomposeItem(const QVariant& data, ffi::PetriNet* net, TreeModel* model, TreeItem* parent);

    void initialize();

    QMenu *contextMenu() override;

    ObjectType item_type() const override {
        return ObjectType::O_Decompose;
    }

    QVariant toVariant() const override;

public slots:

    void onSynthesis(bool checked);

private:

    QAction* m_synthesis = nullptr;

    ffi::DecomposeContext* m_ctx = nullptr;
};

class PrimitiveSystemItem : public TreeItem {
public:
    explicit PrimitiveSystemItem(ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);
    explicit PrimitiveSystemItem(const QVariant& data, ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);

    void initialize();

    ObjectType item_type() const override {
        return ObjectType::O_PrimitiveSystem;
    }

    QVariant toVariant() const override;

private:
    ffi::PetriNet* m_net;
    GraphicScene* m_scene;
};

class LinearBaseFragmentsItem : public TreeItem {
public:
    explicit LinearBaseFragmentsItem(ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);
    explicit LinearBaseFragmentsItem(const QVariant& data, ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);

    void initialize();

    ObjectType item_type() const override {
        return ObjectType::O_LinearBaseFragments;
    }

    QVariant toVariant() const override;

private:
    ffi::PetriNet* m_net;
    GraphicScene* m_scene;
};

class SynthesisItem : public TreeItem {
public:
    explicit SynthesisItem(ffi::SynthesisContext* ctx, TreeModel* model, TreeItem* parent = nullptr);
    explicit SynthesisItem(const QVariant& data, ffi::DecomposeContext* ctx, TreeModel* model, TreeItem* parent = nullptr);

    ObjectType item_type() const override {
        return ObjectType::O_Synthesis;
    }

    QVariant toVariant() const override;

private:
    ffi::SynthesisContext* m_ctx = nullptr;
};

class MatrixItem : public TreeItem {

public:
    explicit MatrixItem(ffi::CMatrix* matrix, TreeModel* model, TreeItem* parent = nullptr);

    ObjectType item_type() const override {
        return ObjectType::O_Matrix;
    }

private:

    ffi::CMatrix* m_matrix = nullptr;

};

class SynthesisProgramsItem : public TreeItem {
public:
    explicit SynthesisProgramsItem(ffi::SynthesisContext* ctx, TreeModel* model, TreeItem* parent = nullptr);
    explicit SynthesisProgramsItem(const QVariant& data, ffi::SynthesisContext* ctx, TreeModel* model, TreeItem* parent = nullptr);

    ObjectType item_type() const override {
        return ObjectType::O_SynthesisPrograms;
    }

    QVariant toVariant() const override;

public slots:

    void onProgramSynthesed(ffi::PetriNet* net);
};

class SynthesisProgramItem : public TreeItem {
public:
    explicit SynthesisProgramItem(ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);
    explicit SynthesisProgramItem(const QVariant& data, ffi::PetriNet* net, TreeModel* model, TreeItem* parent = nullptr);


    ObjectType item_type() const override {
        return ObjectType::O_SynthesisProgram;
    }

    QVariant toVariant() const override;

private:

    GraphicScene* m_scene;
    ffi::PetriNet* m_net;
};

#endif // TREEITEM_H
