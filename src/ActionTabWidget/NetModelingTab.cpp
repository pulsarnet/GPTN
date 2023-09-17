#include <QBoxLayout>
#include "NetModelingTab.h"
#include "../Editor/GraphicsScene.h"
#include "../Editor/elements/position.h"
#include "../Editor/elements/transition.h"
#include "../Editor/elements/arrow_line.h"
#include "../Core/ProjectMetadata.h"

NetModelingTab::NetModelingTab(ProjectMetadata* metadata, QWidget *parent)
    : QWidget(parent)
    ,m_metadata(metadata)
    ,m_view(new GraphicsView(this))
{
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

    auto scene = new GraphicsScene(metadata->context()->net());
    scene->setAllowMods(GraphicsScene::A_Default);

    m_view->setScene(scene);

    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);
}

ProjectMetadata *NetModelingTab::metadata() const {
    return m_metadata;
}

GraphicsView* NetModelingTab::view() const {
    return m_view;
}