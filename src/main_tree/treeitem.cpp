#include "treeitem.h"

#include "../view/graphic_scene.h"
#include "../view/graphics_view.h"
#include "../synthesis/synthesis_table.h"
#include "../ffi/rust.h"
#include "treemodel.h"
#include "../matrix_model.h"
#include <QTableView>

#include <QMenu>

TreeItem::TreeItem(TreeModel* model, TreeItem* parent)
    : QObject(), m_widget(nullptr), m_model(model), m_parentItem(parent)
{
    m_parentItem = parent;

    if (m_parentItem)
        m_parentItem->addChild(this);

}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

QString TreeItem::data(int column) const
{
    Q_UNUSED(column)
    return m_name;
}

void TreeItem::addChild(TreeItem *child)
{
    auto it = std::find_if(m_childItems.begin(), m_childItems.end(), [=](TreeItem* item) {
        return item == child;
    });

    if (it == m_childItems.end()) {
        beginInsertRows();
        m_childItems.push_back(child);
        endInsertRows();
    }
}

void TreeItem::removeChild(TreeItem *child)
{
    auto it = std::find_if(m_childItems.begin(), m_childItems.end(), [=](TreeItem* item) {
        return item == child;
    });

    if (it != m_childItems.end()) {
        m_childItems.erase(it, it);
        delete *it;
    }
}

TreeItem *TreeItem::child(int row) const
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

void TreeItem::setDockWidget(ads::CDockWidget* widget) {
    m_widget = widget;
}

ads::CDockWidget* TreeItem::dockWidget() const {
    return m_widget;
}

void TreeItem::setName(const QString &name)
{
    m_name = name;
}

void TreeItem::beginInsertRows() {
    auto parent = parentItem();
    auto count = childCount();
    model()->emitBeginInsertRows(parent ? model()->indexForTreeItem(this) : QModelIndex(), count, count);
}

void TreeItem::endInsertRows() {
    model()->emitEndInsertRows();
}

const QString &TreeItem::name() const
{
    return m_name;
}

ads::CDockManager *TreeItem::dockManager() const {
    return m_model->dockManager();
}

TreeModel *TreeItem::model() const {
    return m_model;
}

TreeItem::~TreeItem()
{
    for (auto item : m_childItems) {
        delete item;
    }
}

RootTreeItem::RootTreeItem(TreeModel* model, TreeItem* parent) : TreeItem(model, parent)
{
    setName("Root");
    m_netCreate = new QAction("Create net");
    connect(m_netCreate, &QAction::triggered, this, &RootTreeItem::onNetCreate);
}

QMenu *RootTreeItem::contextMenu()
{
    auto menu = new QMenu;
    menu->addAction(m_netCreate);
    return menu;
}

void RootTreeItem::onNetCreate(bool checked)
{
    Q_UNUSED(checked)
    addChild(new NetTreeItem(model(), this));
}

QVariant RootTreeItem::toVariant() const {
    QVariantList children;
    for (int i = 0; i < childCount(); i++) {
        children << child(i)->toVariant();
    }
    return children;
}

void RootTreeItem::fromVariant(const QVariant& data) {
    auto children = data.toList();
    for (auto & i : children) {
        auto child = i.toHash();
        if (child["type"] == O_Net)
            this->addChild(new NetTreeItem(child, model(), this));
    }
}

NetTreeItem::NetTreeItem(TreeModel* model, TreeItem* parent): TreeItem(model, parent)
{
    setName("Net tree");
    m_decompose = new QAction("Decompose", this);
    connect(m_decompose, &QAction::triggered, this, &NetTreeItem::onDecompose);

    auto view = new GraphicsView;
    m_scene = new GraphicScene;
    m_scene->setAllowMods(GraphicScene::A_Default);
    view->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Petri net"));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

NetTreeItem::NetTreeItem(const QVariant &data, TreeModel *model, TreeItem *parent): TreeItem(model, parent) {
    m_decompose = new QAction("Decompose", this);
    connect(m_decompose, &QAction::triggered, this, &NetTreeItem::onDecompose);

    auto map = data.toHash();
    setName(map["name"].toString());

    auto net = ffi::PetriNet::create();
    net->fromVariant(map["net"]);

    auto view = new GraphicsView;
    m_scene = new GraphicScene(map["scene"], net);
    m_scene->setAllowMods(GraphicScene::A_Default);
    view->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Petri net"));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

QMenu *NetTreeItem::contextMenu()
{
    auto menu = new QMenu;
    menu->addAction(m_decompose);
    return menu;
}

void NetTreeItem::onDecompose(bool checked)
{
    Q_UNUSED(checked)

    auto decomposeContext = ffi::DecomposeContext::init(m_scene->net());

    addChild(new DecomposeItem(decomposeContext, model(), this));
}

QVariant NetTreeItem::toVariant() const {
    QVariantHash net;
    net["type"] = item_type();
    net["name"] = name();
    net["net"] = m_scene->net()->toVariant();
    net["scene"] = m_scene->toVariant();
    return net;
}

DecomposeItem::DecomposeItem(ffi::DecomposeContext* ctx, TreeModel* _model, TreeItem *parent) : TreeItem(_model, parent), m_ctx(ctx)
{
    setName("Decompose");
    m_synthesis = new QAction("Synthesis");
    connect(m_synthesis, &QAction::triggered, this, &DecomposeItem::onSynthesis);

    addChild(new PrimitiveSystemItem(m_ctx->primitive_net(), model(), this));
    addChild(new LinearBaseFragmentsItem(m_ctx->linear_base_fragments(), model(), this));


}

QMenu *DecomposeItem::contextMenu()
{
    auto menu = new QMenu;
    menu->addAction(m_synthesis);
    return menu;
}

void DecomposeItem::onSynthesis(bool checked)
{
    Q_UNUSED(checked)

    addChild(new SynthesisItem(ffi::SynthesisContext::init(m_ctx), model(), this));
}


PrimitiveSystemItem::PrimitiveSystemItem(ffi::PetriNet* net, TreeModel* _model, TreeItem *parent): TreeItem(_model, parent), m_net(net)
{
    setName("Primitive system");
    auto view = new GraphicsView;
    m_scene = new GraphicScene();
    m_scene->loadFromNet(m_net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Primitive View"));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

LinearBaseFragmentsItem::LinearBaseFragmentsItem(ffi::PetriNet* net, TreeModel* model, TreeItem *parent): TreeItem(model, parent), m_net(net)
{
    setName("Linear base fragments");
    auto view = new GraphicsView;
    m_scene = new GraphicScene;
    m_scene->loadFromNet(m_net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Linear base fragments"));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

SynthesisItem::SynthesisItem(ffi::SynthesisContext* ctx, TreeModel* _model, TreeItem *parent): TreeItem(_model, parent), m_ctx(ctx)
{
    setName("Synthesis");
    addChild(new MatrixItem(ctx->c_matrix(), model(), this));
    addChild(new SynthesisProgramsItem(m_ctx, model(), this));
}

MatrixItem::MatrixItem(ffi::CMatrix* matrix, TreeModel* model, TreeItem *parent): TreeItem(model, parent)
{
    setName("Matrix");
    auto view = new QTableView;
    view->setModel(MatrixModel::loadFromMatrix(matrix));

    setDockWidget(new ads::CDockWidget(name()));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

SynthesisProgramsItem::SynthesisProgramsItem(ffi::SynthesisContext* ctx, TreeModel* model, TreeItem *parent): TreeItem(model, parent)
{
    setName("Programs");
    auto view = new SynthesisTable(ctx);
    connect(view, &SynthesisTable::signalSynthesisedProgram, this, &SynthesisProgramsItem::onProgramSynthesed);

    setDockWidget(new ads::CDockWidget(name()));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

void SynthesisProgramsItem::onProgramSynthesed(ffi::PetriNet *net) {
    addChild(new SynthesisProgramItem(net, model(), this));
}


SynthesisProgramItem::SynthesisProgramItem(ffi::PetriNet *net, TreeModel *model, TreeItem *parent): TreeItem(model, parent), m_net(net) {
    setName("Program result");

    auto view = new GraphicsView;
    m_scene = new GraphicScene;
    m_scene->loadFromNet(m_net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Program result"));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}