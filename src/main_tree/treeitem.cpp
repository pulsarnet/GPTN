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
    : QObject(parent), m_widget(nullptr), m_model(model), m_parentItem(parent)
{
    m_parentItem = parent;

    if (m_parentItem)
        m_parentItem->addChild(this);

}

int TreeItem::row() const
{
    if (m_parentItem)
        return (int)m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

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

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete m_childItems.takeAt(position);

    return true;
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

    if (m_widget) {
        m_model->dockManager()->removeDockWidget(m_widget);
        delete m_widget;
    }
}

GraphicsViewTreeItem::GraphicsViewTreeItem(TreeModel *model, TreeItem *parent) : TreeItem(model, parent), m_view(new GraphicsView) {

}

GraphicsViewTreeItem::~GraphicsViewTreeItem() noexcept {
    delete m_view;
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
    QVariantList nets;
    for (int i = 0; i < childCount(); i++) {
        nets << child(i)->toVariant();
    }
    return nets;
}

void RootTreeItem::fromVariant(const QVariant& data) {
    auto nets = data.toList();
    for (auto & net : nets) {
        this->addChild(new NetTreeItem(net, model(), this));
    }
}

NetTreeItem::NetTreeItem(TreeModel* model, TreeItem* parent): GraphicsViewTreeItem(model, parent)
{
    setName("Net tree");
    m_scene = new GraphicScene(ffi::PetriNet::create());
    m_scene->setAllowMods(GraphicScene::A_Default);

    initialize();
}

NetTreeItem::NetTreeItem(const QVariant &data, TreeModel *model, TreeItem *parent): GraphicsViewTreeItem(model, parent) {
    auto map = data.toHash();
    setName(map["name"].toString());

    auto net = ffi::PetriNet::create();
    net->fromVariant(map["net"]);

    m_scene = new GraphicScene(map["scene"], net);
    m_scene->setAllowMods(GraphicScene::A_Default);

    auto children = map["children"].toList();
    for (auto child : children) {
        auto childHash = child.toHash();
        if (childHash["type"] == O_Decompose) {
            this->addChild(new DecomposeItem(childHash, net, model, this));
        }
    }

    initialize();
}

void NetTreeItem::initialize() {
    m_decompose = new QAction("Decompose", this);
    connect(m_decompose, &QAction::triggered, this, &NetTreeItem::onDecompose);

    view()->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Petri net"));
    dockWidget()->setWidget(view());
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

ffi::PetriNet *NetTreeItem::net() const {
    return m_scene->net();
}

QVariant NetTreeItem::toVariant() const {
    QVariantHash net;
    net["type"] = item_type();
    net["name"] = name();
    net["net"] = m_scene->net()->toVariant();
    net["scene"] = m_scene->toVariant();

    QVariantList children;
    for (int i = 0; i < childCount(); i++) {
        children << child(i)->toVariant();
    }

    net["children"] = children;

    return net;
}

NetTreeItem::~NetTreeItem() noexcept {
    auto net = m_scene->net();
    delete m_scene;
    net->drop();
}

DecomposeItem::DecomposeItem(ffi::DecomposeContext* ctx, TreeModel* _model, TreeItem *parent) : TreeItem(_model, parent), m_ctx(ctx)
{
    setName("Decompose");
    addChild(new PrimitiveSystemItem(m_ctx->primitive_net(), model(), this));
    addChild(new LinearBaseFragmentsItem(m_ctx->linear_base_fragments(), model(), this));

    initialize();
}

DecomposeItem::DecomposeItem(const QVariant& data, ffi::PetriNet* net, TreeModel *_model, TreeItem *parent) : TreeItem(_model, parent), m_ctx(nullptr)
{
    auto map = data.toHash();
    setName(map["name"].toString());

    auto parts = map["parts"].toList();
    auto array = QVector<ffi::PetriNet*>();
    for (const auto& part : parts) {
        auto net = ffi::PetriNet::create();
        net->fromVariant(part);
        array.push_back(net);
    }

    m_ctx = ffi::DecomposeContext::fromNets(std::move(array));

    auto children = map["children"].toList();
    for (const auto& child : children) {
        auto childHash = child.toHash();
        if (childHash["type"].toInt() == O_PrimitiveSystem) {
            addChild(new PrimitiveSystemItem(childHash, m_ctx->primitive_net(), model(), this));
        }
        else if (childHash["type"].toInt() == O_LinearBaseFragments) {
            addChild(new LinearBaseFragmentsItem(childHash, m_ctx->linear_base_fragments(), model(), this));
        }
        else if (childHash["type"].toInt() == O_Synthesis) {
            addChild(new SynthesisItem(childHash, m_ctx, model(), this));
        }
    }

    initialize();
}

void DecomposeItem::initialize() {
    m_synthesis = new QAction("Synthesis");
    connect(m_synthesis, &QAction::triggered, this, &DecomposeItem::onSynthesis);
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

QVariant DecomposeItem::toVariant() const {

    QVariantHash result;
    QVariantList partsVariant;

    result["type"] = item_type();
    result["name"] = name();

    auto parts = m_ctx->parts();
    for (int i = 0; i < parts.size(); i++) {
        partsVariant << parts[i]->toVariant();
    }

    result["parts"] = partsVariant;

    QVariantList children;
    for (int i = 0; i < childCount(); i++) {
        children << child(i)->toVariant();
    }
    result["children"] = children;

    return result;

}

DecomposeItem::~DecomposeItem() noexcept {
    m_ctx->drop();
    delete m_synthesis;
}

PrimitiveSystemItem::PrimitiveSystemItem(ffi::PetriNet* net, TreeModel* _model, TreeItem *parent): GraphicsViewTreeItem(_model, parent), m_net(net)
{
    setName("Primitive system");
    m_scene = new GraphicScene(m_net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

PrimitiveSystemItem::PrimitiveSystemItem(const QVariant &data, ffi::PetriNet *net, TreeModel *_model, TreeItem *parent): GraphicsViewTreeItem(_model, parent), m_net(net)
{
    auto map = data.toHash();

    setName(map["name"].toString());

    m_scene = new GraphicScene(map["scene"], net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

void PrimitiveSystemItem::initialize() {
    view()->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Primitive View"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

QVariant PrimitiveSystemItem::toVariant() const {
    QVariantHash result;
    result["type"] = item_type();
    result["name"] = name();
    result["scene"] = m_scene->toVariant();
    return result;
}

PrimitiveSystemItem::~PrimitiveSystemItem() noexcept {
    delete m_scene;
}

LinearBaseFragmentsItem::LinearBaseFragmentsItem(ffi::PetriNet* net, TreeModel* model, TreeItem *parent): GraphicsViewTreeItem(model, parent), m_net(net)
{
    setName("Linear base fragments");
    m_scene = new GraphicScene(m_net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

LinearBaseFragmentsItem::LinearBaseFragmentsItem(const QVariant &data, ffi::PetriNet *net, TreeModel *model, TreeItem *parent) : GraphicsViewTreeItem(model, parent), m_net(net)
{
    auto map = data.toHash();
    setName("Linear base fragments");
    m_scene = new GraphicScene(map["scene"], net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

void LinearBaseFragmentsItem::initialize() {

    view()->setScene(m_scene);
    setDockWidget(new ads::CDockWidget("Linear base fragments"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

QVariant LinearBaseFragmentsItem::toVariant() const {
    QVariantHash result;
    result["type"] = item_type();
    result["name"] = name();
    result["scene"] = m_scene->toVariant();
    return result;
}

LinearBaseFragmentsItem::~LinearBaseFragmentsItem() noexcept {
    delete m_scene;
}

SynthesisItem::SynthesisItem(ffi::SynthesisContext* ctx, TreeModel* _model, TreeItem *parent): TreeItem(_model, parent), m_ctx(ctx)
{
    setName("Synthesis");
    addChild(new MatrixItem(ctx->c_matrix(), model(), this));
    addChild(new SynthesisProgramsItem(m_ctx, model(), this));
}

SynthesisItem::SynthesisItem(const QVariant &data, ffi::DecomposeContext *ctx, TreeModel *_model, TreeItem *parent): TreeItem(_model, parent)
, m_ctx(ffi::SynthesisContext::create(ctx))
{
    auto map = data.toHash();
    setName(map["name"].toString());
    m_ctx->fromVariant(map["data"]);

    addChild(new MatrixItem(m_ctx->c_matrix(), model(), this));

    auto children = map["children"].toList();
    for (auto& ch : children) {
        auto childHash = ch.toHash();
        if (childHash["type"].toInt() == O_SynthesisPrograms) {
            addChild(new SynthesisProgramsItem(childHash, m_ctx, model(), this));
        }
    }
}

QVariant SynthesisItem::toVariant() const {
    QVariantHash result;
    result["type"] = item_type();
    result["name"] = name();
    result["data"] = m_ctx->toVariant();

    QVariantList children;
    for (int i = 0; i < childCount(); i++) {
        if (child(i)->item_type() == O_Matrix)
            continue;

        children << child(i)->toVariant();
    }
    result["children"] = children;

    return result;
}

SynthesisItem::~SynthesisItem() noexcept {
    m_ctx->drop();
}

MatrixItem::MatrixItem(ffi::CMatrix* matrix, TreeModel* model, TreeItem *parent): TreeItem(model, parent), m_matrix(matrix)
{
    setName("Matrix");
    m_view = new QTableView;
    m_view->setModel(MatrixModel::loadFromMatrix(matrix));

    setDockWidget(new ads::CDockWidget(name()));
    dockWidget()->setWidget(m_view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

MatrixItem::~MatrixItem() noexcept {
    delete m_view;
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

SynthesisProgramsItem::SynthesisProgramsItem(const QVariant &data, ffi::SynthesisContext *ctx, TreeModel *_model, TreeItem *parent) : TreeItem(_model, parent)
{
    setName("Programs");
    auto view = new SynthesisTable(ctx);
    connect(view, &SynthesisTable::signalSynthesisedProgram, this, &SynthesisProgramsItem::onProgramSynthesed);

    setDockWidget(new ads::CDockWidget(name()));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());

    auto map = data.toMap();
    setName(map["name"].toString());

    auto programsList = map["children"].toList();
    for (int i = 0; i < programsList.size(); i++) {
        addChild(new SynthesisProgramItem(programsList[i], ctx->program_net_after(i), model(), this));
    }
}

void SynthesisProgramsItem::onProgramSynthesed(ffi::PetriNet *net) {
    addChild(new SynthesisProgramItem(net, model(), this));
}

QVariant SynthesisProgramsItem::toVariant() const {
    QVariantHash result;
    result["type"] = item_type();
    result["name"] = name();

    QVariantList children;
    for (int i = 0; i < childCount(); i++) {
        auto variant = child(i)->toVariant();
        if (!variant.isNull())
            children << variant;
    }
    result["children"] = children;

    return result;
}

SynthesisProgramItem::SynthesisProgramItem(ffi::PetriNet *net, TreeModel *model, TreeItem *parent): GraphicsViewTreeItem(model, parent), m_net(net) {
    setName("Program result");

    m_scene = new GraphicScene(m_net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view()->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Program result"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

SynthesisProgramItem::SynthesisProgramItem(const QVariant &data, ffi::PetriNet *net, TreeModel *model, TreeItem *parent): GraphicsViewTreeItem(model, parent), m_net(net)
{
    setName("Program result");

    m_scene = new GraphicScene(data.toHash()["scene"], net);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view()->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Program result"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

QVariant SynthesisProgramItem::toVariant() const {
    QVariantHash result;
    result["type"] = item_type();
    result["name"] = name();
    result["scene"] = m_scene->toVariant();

    return result;
}