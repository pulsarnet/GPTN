#include <QBoxLayout>
#include "NetModelingTab.h"
#include "../Editor/GraphicsScene.h"
#include "../Editor/elements/position.h"
#include "../Editor/elements/transition.h"
#include "../Editor/elements/arrow_line.h"
#include "../MainWindow.h"

NetModelingTab::NetModelingTab(MainWindow* window, QWidget *parent)
    : QWidget(parent)
    ,m_mainWindow(window)
    ,m_view(new GraphicsView(window, this))
{
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

    auto scene = new GraphicsScene(m_mainWindow->metadata()->context()->net());
    scene->setAllowMods(GraphicsScene::A_Default);

    m_view->setScene(scene);

    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);
}


GraphicsView* NetModelingTab::view() const {
    return m_view;
}