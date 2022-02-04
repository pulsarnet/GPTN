#include "treeitem.h"

#include "../view/graphic_scene.h"
#include "../view/graphics_view.h"

#include <QMenu>

TreeItem::TreeItem(ads::CDockManager* manager, TreeItem* parent)
    : QObject(), m_widget(nullptr), m_manager(manager), m_parentItem(parent)
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
        m_childItems.push_back(child);
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

const QString &TreeItem::name() const
{
    return m_name;
}


TreeItem::~TreeItem()
{
    for (auto item : m_childItems) {
        delete item;
    }
}

RootTreeItem::RootTreeItem(ads::CDockManager* manager, TreeItem* parent) : TreeItem(manager, parent)
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

    addChild(new NetTreeItem(dockManager(), this));
}


NetTreeItem::NetTreeItem(ads::CDockManager* manager, TreeItem* parent): TreeItem(manager, parent)
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

QMenu *NetTreeItem::contextMenu()
{
    auto menu = new QMenu;
    menu->addAction(m_decompose);
    return menu;
}

void NetTreeItem::onDecompose(bool checked)
{
    Q_UNUSED(checked)

    auto synthesisContext = ffi::SynthesisContext::init(m_scene->net());

    addChild(new DecomposeItem(synthesisContext, dockManager(), this));
}


DecomposeItem::DecomposeItem(ffi::SynthesisContext* ctx, ads::CDockManager* manager, TreeItem *parent) : TreeItem(manager, parent), m_ctx(ctx)
{
    setName("Decompose");
    m_synthesis = new QAction("Synthesis");
    connect(m_synthesis, &QAction::triggered, this, &DecomposeItem::onSynthesis);

    addChild(new PrimitiveSystemItem(m_ctx->primitive_net(), dockManager(), this));
    addChild(new LinearBaseFragmentsItem(m_ctx->linear_base_fragments(), dockManager(), this));
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

    addChild(new SynthesisItem(dockManager(), this));
}

PrimitiveSystemItem::PrimitiveSystemItem(ffi::PetriNet* net, ads::CDockManager* manager, TreeItem *parent): TreeItem(manager, parent), m_net(net)
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

QMenu *PrimitiveSystemItem::contextMenu()
{
    return nullptr;
}

LinearBaseFragmentsItem::LinearBaseFragmentsItem(ffi::PetriNet* net, ads::CDockManager* manager, TreeItem *parent): TreeItem(manager, parent), m_net(net)
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

QMenu *LinearBaseFragmentsItem::contextMenu()
{
    return nullptr;
}

SynthesisItem::SynthesisItem(ads::CDockManager* manager, TreeItem *parent): TreeItem(manager, parent)
{
    setName("Synthesis");
    addChild(new MatrixItem(dockManager(), this));
    addChild(new SynthesisProgramsItem(dockManager(), this));
}

MatrixItem::MatrixItem(ads::CDockManager* manager, TreeItem *parent): TreeItem(manager, parent)
{
    setName("Matrix");
}

SynthesisProgramsItem::SynthesisProgramsItem(ads::CDockManager* manager, TreeItem *parent): TreeItem(manager, parent)
{
    setName("Programs");
}