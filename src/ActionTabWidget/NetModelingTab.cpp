#include <QBoxLayout>
#include <QSplitter>
#include <ptn/context.h>
#include "NetModelingTab.h"
#include "../Editor/GraphicsScene.h"
#include "../Editor/GraphicsView.h"
#include "../Editor/elements/Transition.h"
#include "../MainWindow.h"
#include <DockAreaWidget.h>
#include "../DockSystem/DockWidget.h"
#include "../Editor/Simulation/SimulationWidget.h"

using namespace ads;

NetModelingTab::NetModelingTab(MainWindow* window, QWidget *parent)
    : BaseTab(parent)
    , m_mainWindow(window)
    , m_dockManager(new CDockManager(this))
{
    /************************* CREATE VIEW *************************/
    auto scene = new GraphicsScene(m_mainWindow->metadata()->net());
    scene->setAllowMods(GraphicsScene::A_Default);
    connect(scene, &GraphicsScene::sceneChanged, m_mainWindow, &MainWindow::onDocumentChanged);
    auto view = new GraphicsView(window);
    view->setScene(scene);
    m_view = new CDockWidget("Modeling");
    m_view->setWidget(view);
    m_view->setFeature(CDockWidget::NoTab, true);

    // add to dock
    m_dockManager->setCentralWidget(m_view);

    /************************* CREATE LAYOUT *************************/
    setLayout(new QGridLayout(this));
    layout()->addWidget(m_dockManager);
    layout()->setContentsMargins(0, 0, 0, 0);
}


GraphicsView* NetModelingTab::view() const {
    return static_cast<GraphicsView*>(m_view->widget());
}