//
// Created by nmuravev on 3/20/2022.
//

#include <QBoxLayout>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
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

QJsonDocument NetModelingTab::json() const {
    auto n_positions = dynamic_cast<GraphicScene*>(m_view->scene())->positions();
    auto n_transitions = dynamic_cast<GraphicScene*>(m_view->scene())->transitions();
    auto n_connections = dynamic_cast<GraphicScene*>(m_view->scene())->connections();

    QJsonArray positions;
    for (auto & n_position : n_positions) {
        auto net_position = n_position->vertex();

        QJsonObject position;
        position.insert("id", QJsonValue((int)net_position->index().id));
        position.insert("markers", QJsonValue((int)net_position->markers()));
        position.insert("parent", QJsonValue((int)net_position->parent()));
        position.insert("name", QJsonValue(net_position->get_name(false)));

        QPointF pos = n_position->pos();
        QJsonObject point;
        point.insert("x", pos.x());
        point.insert("y", pos.y());
        position.insert("pos", point);

        positions.push_back(position);
    }

    QJsonArray transitions;
    for (auto & n_transition : n_transitions) {
        auto net_transition = n_transition->vertex();

        QJsonObject transition;
        transition.insert("id", QJsonValue((int)net_transition->index().id));
        transition.insert("parent", QJsonValue((int)net_transition->parent()));
        transition.insert("name", QJsonValue(net_transition->get_name(false)));

        QPointF pos = n_transition->center();
        QJsonObject point;
        point.insert("x", pos.x());
        point.insert("y", pos.y());
        transition.insert("pos", point);

        transitions.push_back(transition);
    }

    QJsonArray connections;
    for (auto & n_connection : n_connections) {
        auto connection_from = n_connection->from();
        auto connection_to = n_connection->to();

        QJsonObject connection;
        QJsonObject from;
        from.insert("type", (int)connection_from->vertex()->index().type);
        from.insert("index", (int)connection_from->vertex()->index().id);

        QJsonObject to;
        to.insert("type", (int)connection_to->vertex()->index().type);
        to.insert("index", (int)connection_to->vertex()->index().id);

        connection.insert("from", from);
        connection.insert("to", to);

        connections.push_back(connection);
    }

    QJsonObject main;
    main.insert("positions", positions);
    main.insert("transitions", transitions);
    main.insert("connections", connections);

    return QJsonDocument(main);
}
