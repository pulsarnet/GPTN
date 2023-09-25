#include <QBoxLayout>
#include "NetModelingTab.h"
#include "../Editor/GraphicsScene.h"
#include "../Editor/elements/Position.h"
#include "../Editor/elements/Transition.h"
#include "../Editor/elements/ArrowLine.h"
#include "../MainWindow.h"

NetModelingTab::NetModelingTab(MainWindow* window, QWidget *parent)
    : QWidget(parent)
    ,m_mainWindow(window)
    ,m_view(new GraphicsView(window, this))
{
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

    auto scene = new GraphicsScene(m_mainWindow->metadata()->context()->net());
    scene->setAllowMods(GraphicsScene::A_Default);
    connect(scene, &GraphicsScene::sceneChanged, m_mainWindow, &MainWindow::onDocumentChanged);

    m_view->setScene(scene);

    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);
}


GraphicsView* NetModelingTab::view() const {
    return m_view;
}