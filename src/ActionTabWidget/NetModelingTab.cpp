#include <QBoxLayout>
#include "NetModelingTab.h"
#include "../view/GraphicScene.h"
#include "../view/elements/position.h"
#include "../view/elements/transition.h"
#include "../view/elements/arrow_line.h"
#include "../Core/ProjectMetadata.h"

NetModelingTab::NetModelingTab(ProjectMetadata* metadata, QWidget *parent)
    : QWidget(parent)
    ,m_metadata(metadata)
    ,m_view(new GraphicsView(this))
{
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

    auto scene = new GraphicScene(metadata->context()->net());
    scene->setAllowMods(GraphicScene::A_Default);

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