//
// Created by nmuravev on 3/20/2022.
//

#include <QBoxLayout>
#include "NetModelingTab.h"
#include "../view/graphic_scene.h"

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

void NetModelingTab::saveState(QVariant &data) const {
    QVariant state;
    m_ctx->saveState(state);
    data.setValue(std::move(state));
}

void NetModelingTab::restoreState(const QVariant &data) {
    m_ctx->restoreState(data);
}

ffi::PetriNetContext *NetModelingTab::ctx() const {
    return m_ctx;
}