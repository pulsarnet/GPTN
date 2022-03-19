#include "treeitem.h"

#include "../view/graphic_scene.h"
#include "../view/graphics_view.h"
#include "../synthesis/synthesis_table.h"
#include "../ffi/rust.h"
#include "treemodel.h"
#include "../matrix_model.h"
#include "../synthesis/synthesis_model.h"
#include "../overrides/MatrixWindow.h"
#include <QTableView>

#include <QMenu>

TreeItem::TreeItem(TreeModel* model, TreeItem* parent)
    : QObject(parent), m_widget(nullptr), m_model(model), m_parentItem(parent)
{
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

        onChanged();
    }
}

bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > m_childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete m_childItems.takeAt(position);

    onChanged();

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
    auto count = childCount();
    model()->emitBeginInsertRows(this->model()->indexForTreeItem(this), childCount(), childCount());
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

void TreeItem::onChanged() {
    emit m_model->dataChanged(QModelIndex(), QModelIndex());
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
    new NetTreeItem(model(), this);
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
        new NetTreeItem(net, model(), this);
    }
}

NetTreeItem::NetTreeItem(TreeModel* model, TreeItem* parent): GraphicsViewTreeItem(model, parent), m_ctx(ffi::PetriNetContext::create())
{
    setName("Net tree");
    m_scene = new GraphicScene(m_ctx->net(), this);
    m_scene->setAllowMods(GraphicScene::A_Default);

    initialize();
}

NetTreeItem::NetTreeItem(const QVariant &data, TreeModel *model, TreeItem *parent): GraphicsViewTreeItem(model, parent), m_ctx(ffi::PetriNetContext::create()) {
    auto map = data.toHash();
    setName(map["name"].toString());

    auto net = m_ctx->net();
    net->fromVariant(map["net"]);

    m_scene = new GraphicScene(map["scene"], net, this);
    m_scene->setAllowMods(GraphicScene::A_Default);

    auto children = map["children"].toList();
    for (auto child : children) {
        auto childHash = child.toHash();
        if (childHash["type"] == O_Decompose) {
            new DecomposeItem(childHash, net, model, this);
        }
    }

    initialize();
}

void NetTreeItem::initialize() {
    m_decompose = new QAction("Decompose", this);
    connect(m_decompose, &QAction::triggered, this, &NetTreeItem::onDecompose);

    m_asMatrix = new QAction("As Matrix", this);
    connect(m_asMatrix, &QAction::triggered, this, &NetTreeItem::onAsMatrix);

    view()->setScene(m_scene);
    connect(m_scene, &GraphicScene::sceneChanged, this, &TreeItem::onChanged);

    setDockWidget(new ads::CDockWidget("Petri net"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

QMenu *NetTreeItem::contextMenu()
{
    auto menu = new QMenu;
    menu->addAction(m_decompose);
    menu->addAction(m_asMatrix);
    return menu;
}

void NetTreeItem::onDecompose(bool checked)
{
    Q_UNUSED(checked)

    m_ctx->decompose();
    new DecomposeItem(m_ctx->decompose_ctx(), model(), this);
}

void NetTreeItem::onAsMatrix(bool checked) const {
    Q_UNUSED(checked)
    auto matrix = net()->as_matrix();
    auto view = new MatrixWindow(matrix.first, matrix.second);
    view->show();
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
    delete m_scene;
    ffi::PetriNetContext::free(m_ctx);
}

DecomposeItem::DecomposeItem(ffi::DecomposeContext* ctx, TreeModel* _model, TreeItem *parent) : TreeItem(_model, parent), m_ctx(ctx)
{
    setName("Decompose");
    new PrimitiveSystemItem(m_ctx->primitive_net(), model(), this);
    new LinearBaseFragmentsItem(m_ctx->linear_base_fragments(), model(), this);
    new MatrixItem(m_ctx->c_matrix(), model(), this);
    new SynthesisProgramsItem(m_ctx, model(), this);

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
            new PrimitiveSystemItem(childHash, m_ctx->primitive_net(), model(), this);
        }
        else if (childHash["type"].toInt() == O_LinearBaseFragments) {
            new LinearBaseFragmentsItem(childHash, m_ctx->linear_base_fragments(), model(), this);
        }
        else if (childHash["type"].toInt() == O_Synthesis) {
            //new SynthesisItem(childHash, m_ctx, model(), this);
        }
    }

    initialize();
}

void DecomposeItem::initialize() {

}

QMenu *DecomposeItem::contextMenu()
{
    auto menu = new QMenu;
    return menu;
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

}

PrimitiveSystemItem::PrimitiveSystemItem(ffi::PetriNet* net, TreeModel* _model, TreeItem *parent): GraphicsViewTreeItem(_model, parent), m_net(net)
{
    setName("Primitive system");
    m_scene = new GraphicScene(m_net, this);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

PrimitiveSystemItem::PrimitiveSystemItem(const QVariant &data, ffi::PetriNet *net, TreeModel *_model, TreeItem *parent): GraphicsViewTreeItem(_model, parent), m_net(net)
{
    auto map = data.toHash();

    setName(map["name"].toString());

    m_scene = new GraphicScene(map["scene"], net, this);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

void PrimitiveSystemItem::initialize() {
    view()->setScene(m_scene);
    connect(m_scene, &GraphicScene::sceneChanged, this, &TreeItem::onChanged);

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
    m_scene = new GraphicScene(m_net, this);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

LinearBaseFragmentsItem::LinearBaseFragmentsItem(const QVariant &data, ffi::PetriNet *net, TreeModel *model, TreeItem *parent) : GraphicsViewTreeItem(model, parent), m_net(net)
{
    auto map = data.toHash();
    setName("Linear base fragments");
    m_scene = new GraphicScene(map["scene"], net, this);
    m_scene->setAllowMods(GraphicScene::A_Nothing);

    initialize();
}

void LinearBaseFragmentsItem::initialize() {

    view()->setScene(m_scene);
    connect(m_scene, &GraphicScene::sceneChanged, this, &TreeItem::onChanged);

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

SynthesisProgramsItem::SynthesisProgramsItem(ffi::DecomposeContext* ctx, TreeModel* model, TreeItem *parent): TreeItem(model, parent)
{
    setName("Programs");
    auto view = new SynthesisTable(ctx);
    connect(view, &SynthesisTable::signalSynthesisedProgram, this, &SynthesisProgramsItem::onProgramSynthesed);

    setDockWidget(new ads::CDockWidget(name()));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

SynthesisProgramsItem::SynthesisProgramsItem(const QVariant &data, ffi::DecomposeContext *ctx, TreeModel *_model, TreeItem *parent) : TreeItem(_model, parent)
{
    setName("Programs");
    auto view = new SynthesisTable(ctx);
    connect(view, &SynthesisTable::signalSynthesisedProgram, this, &SynthesisProgramsItem::onProgramSynthesed);
    connect(view->model(), &QAbstractItemModel::dataChanged, this, &TreeItem::onChanged);

    setDockWidget(new ads::CDockWidget(name()));
    dockWidget()->setWidget(view);
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());

    auto map = data.toMap();
    setName(map["name"].toString());

    auto programsList = map["children"].toList();
    for (int i = 0; i < programsList.size(); i++) {
        new SynthesisProgramItem(programsList[i], ctx->program_net_after(i), model(), this);
    }
}

void SynthesisProgramsItem::onProgramSynthesed(ffi::PetriNet *net, int index) {

    for (auto i = 0; i < childCount(); i++) {
        auto ch = dynamic_cast<SynthesisProgramItem*>(child(i));
        if (index == ch->index())
            return;
    }

    auto synthesedProgram = new SynthesisProgramItem(net, model(), this);
    synthesedProgram->setIndex(index);
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

SynthesisProgramItem::SynthesisProgramItem(ffi::PetriNet *net, TreeModel *model, TreeItem *parent): GraphicsViewTreeItem(model, parent)
    , m_net(net)
    , m_programTableIndex(-1)
{
    setName("Program result");

    m_scene = new GraphicScene(m_net, this);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view()->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Program result"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

SynthesisProgramItem::SynthesisProgramItem(const QVariant &data, ffi::PetriNet *net, TreeModel *model, TreeItem *parent): GraphicsViewTreeItem(model, parent)
    , m_net(net)
    , m_programTableIndex(-1)
{
    setName("Program result");

    auto map = data.toHash();
    m_programTableIndex = map["index"].toInt();

    m_scene = new GraphicScene(map["scene"], net, this);
    m_scene->setAllowMods(GraphicScene::A_Nothing);
    view()->setScene(m_scene);

    setDockWidget(new ads::CDockWidget("Program result"));
    dockWidget()->setWidget(view());
    dockManager()->addDockWidgetTab(ads::DockWidgetArea::CenterDockWidgetArea, dockWidget());
}

void SynthesisProgramItem::setIndex(int index) {
    m_programTableIndex = index;
}

int SynthesisProgramItem::index() const {
    return m_programTableIndex;
}

QVariant SynthesisProgramItem::toVariant() const {
    QVariantHash result;
    result["type"] = item_type();
    result["name"] = name();
    result["index"] = index();
    result["scene"] = m_scene->toVariant();

    return result;
}