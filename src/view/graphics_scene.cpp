//
// Created by Николай Муравьев on 13.01.2022.
//

#include "../../include/view/graphics_scene.h"

GraphicScene::GraphicScene(QObject *parent) : QGraphicsScene(parent) {
    m_mod = Mode::A_Nothing;
    m_allowMods = Mode::A_Nothing;
    m_net = make();

    setSceneRect(-12500, -12500, 25000, 25000);
}


void GraphicScene::setMode(Mode mod) {
    if (m_allowMods & mod) m_mod = mod;
    else qDebug() << "Mode " << mod << " not allowed!";
}

void GraphicScene::setAllowMods(Modes mods) {
    m_allowMods = mods;
}

void GraphicScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    switch (m_mod) {
        case A_Position:
            insertPosition(event);
            break;
        case A_Transition:
            insertTransition(event);
            break;
        case A_Remove:
            removeObject(event);
            break;
        case A_Connection:
            connectionStart(event);
            break;
        case A_Move:
            QGraphicsScene::mousePressEvent(event);
            break;
        case A_Rotation:
            rotateObject(event);
            break;
        default:
            break;
    }

    //QGraphicsScene::mousePressEvent(event);
}

void GraphicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    switch (m_mod) {
        case A_Connection:
            if (m_currentConnection) m_currentConnection->setLine(QLineF(m_currentConnection->line().p1(), event->scenePos()));
            break;
        case A_Move:
            QGraphicsScene::mouseMoveEvent(event);
            updateConnections();
            break;
        default:
            break;
    }

    //QGraphicsScene::mouseMoveEvent(event);
}


void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    switch (m_mod) {
        case A_Connection:
            connectionEnd(event);
            break;
        default:
            break;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}


void GraphicScene::insertPosition(QGraphicsSceneMouseEvent *event) {

    auto position = new Position(event->scenePos(), m_net->add_position());
    m_positions.push_back(position);
    addItem(position);

    emit itemInserted(position);

}

void GraphicScene::insertTransition(QGraphicsSceneMouseEvent *event) {

    auto transition = new Transition(event->scenePos(), m_net->add_transition());
    m_transition.push_back(transition);
    addItem(transition);

    emit itemInserted(transition);

}

void GraphicScene::removeObject(QGraphicsSceneMouseEvent *event) {

    auto item = itemAt(event->scenePos(), QTransform());
    if (auto position = dynamic_cast<Position*>(item); position) {
        auto index = m_positions.indexOf(position);
        m_positions.removeAt(index);
        removeConnectionsAssociatedWith(position);
        m_net->remove_position(position->position());
        delete position;
    }
    else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        auto index = m_transition.indexOf(transition);
        m_transition.removeAt(index);
        removeConnectionsAssociatedWith(transition);
        m_net->remove_transition(transition->transition());
        delete transition;
    }
    else if (auto connection_line = dynamic_cast<ArrowLine*>(item); connection_line) {
        connection_line->disconnect(m_net);
        removeItem(connection_line);
        m_connections.removeAt(m_connections.indexOf(connection_line));
        delete connection_line;
    }

}

void GraphicScene::connectionStart(QGraphicsSceneMouseEvent *event) {

    auto item = itemAt(event->scenePos(), QTransform());
    if (auto petri = dynamic_cast<PetriObject*>(item); petri) {
        auto connection = new ArrowLine(petri, QLineF(item->scenePos(), event->scenePos()));
        addItem(connection);
        m_currentConnection = connection;
    }

}

void GraphicScene::connectionEnd(QGraphicsSceneMouseEvent *event) {

    if (!m_currentConnection) return;

    auto item = netItemAt(event->scenePos());
    if (auto petri = dynamic_cast<PetriObject*>(item); petri) {
        if (m_currentConnection->from()->allowConnection(petri)) {
            m_currentConnection->setTo(petri);
            m_currentConnection->setLine(
                    QLineF(m_currentConnection->from()->connectionPos(petri, true),
                           petri->connectionPos(m_currentConnection->from(), false)));
            m_connections.push_back(m_currentConnection);

            if (m_currentConnection->from()->objectType() == PetriObject::Position)
                m_net->connect_p(dynamic_cast<Position*>(m_currentConnection->from())->position(),
                                 dynamic_cast<Transition*>(m_currentConnection->to())->transition());
            else
                m_net->connect_t(dynamic_cast<Transition*>(m_currentConnection->from())->transition(),
                                 dynamic_cast<Position*>(m_currentConnection->to())->position());

            m_currentConnection = nullptr;
        }
    }

    if (m_currentConnection) {
        removeItem(m_currentConnection);
        m_currentConnection = nullptr;
    }

}

void GraphicScene::removeConnectionsAssociatedWith(PetriObject *object) {

    QMutableListIterator iter(m_connections);
    while (iter.hasNext()) {
        auto connection = iter.next();
        if (connection->from() == object || connection->to() == object) {
            removeItem(connection);
            iter.remove();
            connection->disconnect(m_net);
            delete connection;
        }
    }

}

void GraphicScene::updateConnections() {
    foreach(auto connection, m_connections) {
        connection->setLine(QLineF(connection->from()->connectionPos(connection->to(), true),
                                   connection->to()->connectionPos(connection->from(), false)));
    }
}

PetriObject *GraphicScene::netItemAt(const QPointF &pos) {
    auto it = std::find_if(m_positions.begin(), m_positions.end(), [&](Position* item) {
       return item->sceneBoundingRect().contains(pos);
    });

    if (it != m_positions.end()) return *it;

    auto t_it = std::find_if(m_transition.begin(), m_transition.end(), [&](Transition* item) {
        return item->sceneBoundingRect().contains(pos);
    });

    if (t_it != m_transition.end()) return *t_it;

    return nullptr;
}

void GraphicScene::rotateObject(QGraphicsSceneMouseEvent *event) {
    auto item = netItemAt(event->scenePos());
    if (item) {
        if (auto transition = dynamic_cast<Transition*>(item); transition)
            transition->setRotation(transition->rotation() == 0 ? 90 : 0);
    }
}

PetriNet *GraphicScene::net() {
    return m_net;
}

QVariant GraphicScene::toVariant() {
    QVariantList data_list;
    QVariantList connections_list;

    for (auto position : m_positions) {
        QVariantHash data;
        data["pos"] = QVariant(position->scenePos());
        data["rotation"] = QVariant(position->rotation());
        data["type"] = QVariant(tr("position"));
        data["id"] = QVariant(position->index());
        data_list << data;
    }

    for (auto transition : m_transition) {
        QVariantHash data;
        data["pos"] = QVariant(transition->scenePos());
        data["rotation"] = QVariant(transition->rotation());
        data["type"] = QVariant(tr("transition"));
        data["id"] = QVariant(transition->index());
        data_list << data;
    }

    for (auto connection : m_connections) {
        QVariantHash from;
        from["type"] = QVariant(connection->from()->objectTypeStr());
        from["id"] = QVariant(connection->from()->index());

        QVariantHash to;
        to["type"] = QVariant(connection->to()->objectTypeStr());
        to["id"] = QVariant(connection->to()->index());

        QVariantHash data;
        data["from"] = QVariant(from);
        data["to"] = QVariant(to);

        connections_list << data;
    }

    QVariantHash common_data;
    common_data["items"] = data_list;
    common_data["connections"] = connections_list;

    return QVariant(common_data);
}

void GraphicScene::fromVariant(QVariant data) {
    auto common_data = data.toHash();

    if (common_data.contains("items")) {
        foreach(QVariant data, common_data.value("items").toList()) {
            QVariantHash hash = data.toHash();
            if (hash["type"] == "position") {
                auto object = new Position(hash["pos"].toPointF(), this->net()->add_position_with(hash["id"].toInt()));
                m_positions.push_back(object);
                addItem(object);
            }
            else if (hash["type"] == "transition") {
                auto object = new Transition(hash["pos"].toPointF(), this->net()->add_transition_with(hash["id"].toInt()));
                object->setRotation(hash["rotation"].toDouble());
                m_transition.push_back(object);
                addItem(object);
            }
        }
    }

    if (common_data.contains("connections")) {
        foreach(QVariant connection, common_data.value("connections").toList()) {
            QVariantHash hash = connection.toHash();

            QVariantHash from = hash["from"].toHash();
            QVariantHash to = hash["to"].toHash();

            qDebug() << from;
            qDebug() << to;

            auto find_from = [=](PetriObject* object) {
                return object->objectTypeStr() == from["type"].toString() && object->index() == from["id"].toInt();
            };

            auto find_to = [=](PetriObject* object) {
                return object->objectTypeStr() == to["type"].toString() && object->index() == to["id"].toInt();
            };

            PetriObject* point1 = nullptr;
            PetriObject* point2 = nullptr;

            if (from["type"] == "position") {
                auto point1_it = std::find_if(m_positions.begin(), m_positions.end(), find_from);
                auto point2_it = std::find_if(m_transition.begin(), m_transition.end(), find_to);

                point1 = dynamic_cast<PetriObject*>(*point1_it);
                point2 = dynamic_cast<PetriObject*>(*point2_it);
            }
            else {
                auto point1_it = std::find_if(m_transition.begin(), m_transition.end(), find_from);
                auto point2_it = std::find_if(m_positions.begin(), m_positions.end(), find_to);

                point1 = dynamic_cast<PetriObject*>(*point1_it);
                point2 = dynamic_cast<PetriObject*>(*point2_it);
            }

            auto line_item = new ArrowLine(point1, QLineF(point1->scenePos().x(), point1->scenePos().y(), point2->scenePos().x(), point2->scenePos().y()));
            line_item->setTo(point2);
            addItem(line_item);
            m_connections.push_back(line_item);
            point1->connectTo(net(), point2);
        }

        updateConnections();
    }
}
