//
// Created by nmuravev on 3/20/2022.
//

#include <QBoxLayout>
#include "NetModelingTab.h"
#include "../view/graphic_scene.h"
#include "../elements/position.h"
#include "../elements/transition.h"
#include "../elements/arrow_line.h"

NetModelingTab::NetModelingTab(QWidget *parent) : QWidget(parent) {
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

    m_ctx = ffi::PetriNetContext::create();
    m_view = new GraphicsView(this);

    auto scene = new GraphicScene(m_ctx->net());
    scene->setAllowMods(GraphicScene::A_Default);

    m_view->setScene(scene);

    layout()->addWidget(m_view);
    layout()->setContentsMargins(0, 0, 0, 0);
}

ffi::PetriNetContext *NetModelingTab::ctx() const {
    return m_ctx;
}

GraphicsView* NetModelingTab::view() const {
    return m_view;
}