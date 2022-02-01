//
// Created by Николай Муравьев on 13.01.2022.
//


#include <QMenu>
#include "graphic_scene.h"
#include "../elements/petri_object.h"
#include "../elements/position.h"
#include "../elements/transition.h"
#include "../elements/arrow_line.h"
#include "../graphviz/graphviz_wrapper.h"


GraphicScene::GraphicScene(QObject *parent) : QGraphicsScene(parent) {
    m_mod = Mode::A_Nothing;
    m_allowMods = Mode::A_Nothing;
    m_net = ffi::PetriNet::create();

    setSceneRect(-12500, -12500, 25000, 25000);

    connect(this, &QGraphicsScene::selectionChanged, this, &GraphicScene::slotSelectionChanged);
}


void GraphicScene::setMode(Mode mod) {
    if (m_allowMods & mod) m_mod = mod;
    else qDebug() << "Mode " << mod << " not allowed!";
}

void GraphicScene::slotSelectionChanged() {



}

void GraphicScene::setAllowMods(Modes mods) {
    m_allowMods = mods;
}

void GraphicScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {

    if (event->button() == Qt::LeftButton) {
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
                if (m_currentConnection) connectionCommit(event);
                else connectionStart(event);
                event->setAccepted(true);
                break;
            case A_Move:
                QGraphicsScene::mousePressEvent(event);
                break;
            case A_Rotation:
                rotateObject(event);
                break;
            case A_Marker:
                markPosition(event);
                break;
            default:
                break;
        }
    }
    else if (event->button() == Qt::RightButton) {
        if (m_mod == A_Connection) connectionRollback(event);
        else QGraphicsScene::mousePressEvent(event);
    }

    //QGraphicsScene::mousePressEvent(event);
}

void GraphicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {

    switch (m_mod) {
        case A_Connection:
            if (m_currentConnection) {
                m_currentConnection->setLine(QLineF(m_currentConnection->line().p1(), event->scenePos()));
                m_currentConnection->updateConnection();
            }
            break;
        case A_Move:
            QGraphicsScene::mouseMoveEvent(event);
            break;
        default:
            break;
    }

    //QGraphicsScene::mouseMoveEvent(event);
}

void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsScene::mouseReleaseEvent(event);
}


void GraphicScene::insertPosition(QGraphicsSceneMouseEvent *event) {
    addPosition(-1, event->scenePos());
}

void GraphicScene::insertTransition(QGraphicsSceneMouseEvent *event) {
    addTransition(-1, event->scenePos());
}

void GraphicScene::removeObject(QGraphicsSceneMouseEvent *event) {

    auto item = itemAt(event->scenePos(), QTransform());
    if (auto position = dynamic_cast<Position*>(item); position) {
        auto index = m_positions.indexOf(position);
        m_positions.removeAt(index);
        removeConnectionsAssociatedWith(position);
        m_net->remove_position(position->vertex());
        delete position;
    }
    else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        auto index = m_transition.indexOf(transition);
        m_transition.removeAt(index);
        removeConnectionsAssociatedWith(transition);
        m_net->remove_transition(transition->vertex());
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

void GraphicScene::connectionCommit(QGraphicsSceneMouseEvent *event) {

    if (!m_currentConnection) return;

    auto item = netItemAt(event->scenePos());
    if (auto petri = dynamic_cast<PetriObject*>(item); petri) {
        if (m_currentConnection->setTo(petri)) {
            m_currentConnection->setLine(
                    QLineF(m_currentConnection->from()->connectionPos(petri, true),
                           petri->connectionPos(m_currentConnection->from(), false)));
            m_currentConnection->updateConnection();
            m_connections.push_back(m_currentConnection);

            m_net->connect(m_currentConnection->from()->vertex(), m_currentConnection->to()->vertex());

            m_currentConnection = nullptr;
        }
    }

    if (m_currentConnection) {
        removeItem(m_currentConnection);
        m_currentConnection = nullptr;
    }

}

void GraphicScene::connectionRollback(QGraphicsSceneMouseEvent *event) {
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

ffi::PetriNet *GraphicScene::net() {
    return m_net;
}

QVariant GraphicScene::toVariant() {
    QVariantList data_list;
    QVariantList connections_list;

    for (auto position : m_positions) {
        QVariantHash data;
        data["pos"] = QVariant(position->scenePos());
        data["name"] = QVariant(position->label());
        data["rotation"] = QVariant(position->rotation());
        data["type"] = QVariant(tr("position"));
        data["id"] = QVariant(position->index());
        data_list << data;
    }

    for (auto transition : m_transition) {
        QVariantHash data;
        data["pos"] = QVariant(transition->scenePos());
        data["name"] = QVariant(transition->label());
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

void GraphicScene::fromVariant(const QVariant& data) {
    auto common_data = data.toHash();

    if (common_data.contains("items")) {
        foreach(QVariant data, common_data.value("items").toList()) {
            QVariantHash hash = data.toHash();
            if (hash["type"] == "position") {
                auto object = addPosition(hash["id"].toInt(), hash["pos"].toPointF());
                object->setLabel(hash["name"].toString());
            }
            else if (hash["type"] == "transition") {
                auto object = addTransition(hash["id"].toInt(), hash["pos"].toPointF());
                object->setLabel(hash["name"].toString());
                object->setRotation(hash["rotation"].toDouble());
            }
        }
    }

    qDebug() << "Positions: " << m_positions[0]->index() << m_positions[1]->index() << m_positions[2]->index();
    qDebug() << "Transitions: " << m_positions[0]->index() << m_positions[1]->index() << m_positions[2]->index();

    if (common_data.contains("connections")) {
        foreach(QVariant connection, common_data.value("connections").toList()) {
            QVariantHash hash = connection.toHash();

            QVariantHash from = hash["from"].toHash();
            QVariantHash to = hash["to"].toHash();

            auto find_from = [=](PetriObject* object) {
                return object->objectTypeStr() == from["type"].toString() && object->index() == from["id"].toInt();
            };

            auto find_to = [=](PetriObject* object) {
                return object->objectTypeStr() == to["type"].toString() && object->index() == to["id"].toInt();
            };

            PetriObject* point1;
            PetriObject* point2;

            if (from["type"].toString() == "position") {
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

            connectItems(point1, point2);
        }

    }
}

void GraphicScene::removeAll() {
    QGraphicsScene::clear();

    // TODO: Удаление сети
    m_net = ffi::PetriNet::create(); // TODO: Надо удалить
    m_transition.clear();
    m_positions.clear();
    m_connections.clear();
}

Position* GraphicScene::addPosition(const QString &name, const QPointF &point) {

    if (!name.startsWith("p")) throw 0;

    auto index = name.mid(1).toInt();
    return addPosition(index, point);
}

Position* GraphicScene::addPosition(int index, const QPointF &point) {

    auto position = new Position(point, index == -1 ? m_net->add_position() : m_net->add_position_with(index));
    m_positions.push_back(position);
    addItem(position);

    emit itemInserted(position);

    return position;
}

Transition* GraphicScene::addTransition(const QString &name, const QPointF &point) {
    if (!name.startsWith("t")) throw 0;

    auto index = name.mid(1).toInt();
    return addTransition(index, point);
}

Transition* GraphicScene::addTransition(int index, const QPointF &point) {
    auto transition = new Transition(point, index == -1 ? m_net->add_transition() : m_net->add_transition_with(index));
    m_transition.push_back(transition);
    addItem(transition);

    emit itemInserted(transition);

    return transition;
}

ArrowLine* GraphicScene::connectItems(PetriObject *from, PetriObject *to) {

    auto arrowLine = new ArrowLine(from, QLineF(from->connectionPos(to, false), to->connectionPos(from, true)));
    arrowLine->setTo(to);

    m_net->connect(from->vertex(), to->vertex());

    m_connections.push_back(arrowLine);
    addItem(arrowLine);

    return arrowLine;
}

void GraphicScene::slotHorizontalAlignment(bool triggered) {
    qreal y = 0;
    int elements = 0;

    for (auto item : selectedItems()) {
        if (dynamic_cast<PetriObject*>(item)) {
            y += item->pos().y();
            elements++;
        }
    }

    if (elements > 0) {
        y /= (qreal)elements;
        for (auto item : selectedItems()) {
            if (dynamic_cast<PetriObject*>(item)) {
                item->setPos(QPointF(item->pos().x(), y));
            }
        }
    }

}

void GraphicScene::slotVerticalAlignment(bool triggered) {
    qreal x = 0;
    int elements = 0;

    for (auto item : selectedItems()) {
        if (dynamic_cast<PetriObject*>(item)) {
            x += item->pos().x();
            elements++;
        }
    }

    if (elements > 0) {
        x /= (qreal)elements;
        for (auto item : selectedItems()) {
            if (dynamic_cast<PetriObject*>(item)) {
                item->setPos(QPointF(x, item->pos().y()));
            }
        }
    }

}

QPointF GraphicScene::getTransitionPos(int index) {
    return getTransition(index)->scenePos();
}

QPointF GraphicScene::getPositionPos(int index) {
    return getPosition(index)->scenePos();
}

void GraphicScene::loadFromNet(ffi::PetriNet *net, const QString& algorithm) {

    removeAll();
    m_net = net;

    GraphVizWrapper graph;

    auto positions = m_net->positions();
    auto transitions = m_net->transitions();
    auto connections = m_net->connections();

    for (int i = 0; i < positions.size(); i++) {
        graph.addCircle(QString("p%1").arg(positions[i]->index()).toLocal8Bit().data(), QSizeF(80, 80));
        m_positions.push_back(new Position(QPointF(0, 0), positions[i]));
        addItem(m_positions.last());
    }

    for (int i = 0; i < transitions.size(); i++) {
        graph.addRectangle(QString("t%1").arg(transitions[i]->index()).toLocal8Bit().data(), QSizeF(120, 60));
        m_transition.push_back(new Transition(QPointF(0, 0), transitions[i]));
        addItem(m_transition.last());
    }

    for (int i = 0; i < connections.size(); i++) {
        auto from = connections[i]->from();
        auto to = connections[i]->to();

        if (from->type() == ffi::VertexType::Position) {
            auto position = getPosition(from->index());
            auto transition = getTransition(to->index());
            graph.addEdge(position->name(), transition->name());
            connectItems(position, transition);
        }
        else {
            auto transition = getTransition(from->index());
            auto position = getPosition(to->index());
            graph.addEdge(position->name(), transition->name());
            connectItems(transition, position);
        }
    }

    auto result = graph.save(algorithm.isEmpty() ? (char*)"sfdp" : algorithm.toLocal8Bit().data());
    for (auto& element : result.elements) {
        auto vertex = getVertex(element.first);
        vertex->setPos(element.second);
    }

}

PetriObject *GraphicScene::getVertex(const QString &name) {
    if (name.startsWith("p")) return getPosition(name.mid(1).toInt());
    return getTransition(name.mid(1).toInt());
}

Transition *GraphicScene::getTransition(int index) {
    auto it = std::find_if(m_transition.begin(), m_transition.end(), [=](Transition* t) {
        return t->index() == index;
    });

    return it == m_transition.end() ? nullptr : *it;
}

Position *GraphicScene::getPosition(int index) {
    auto it = std::find_if(m_positions.begin(), m_positions.end(), [=](Position* p) {
        return p->index() == index;
    });

    return it == m_positions.end() ? nullptr : *it;
}

void GraphicScene::markPosition(QGraphicsSceneMouseEvent *event) {
    auto item = netItemAt(event->scenePos());
    if (auto position = dynamic_cast<Position*>(item); position) {
        if (event->button() == Qt::LeftButton) position->add_marker();
        else if (event->button() == Qt::RightButton) position->remove_marker();
        position->update();
    }
}

void GraphicScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (m_mod == A_Move) QGraphicsScene::mouseDoubleClickEvent(event);
}


void GraphicScene::dotVisualization(char* algorithm) {

    GraphVizWrapper graph;
    for (auto& element : this->positions()) {
        graph.addCircle(element->name().toLocal8Bit().data(), QSizeF(50., 50.));
    }

    for (auto& element : this->transitions()) {
        graph.addRectangle(element->name().toLocal8Bit().data(), QSizeF(30., 90.));
    }

    for (auto& conn : this->connections()) {
        graph.addEdge(conn->from()->name(), conn->to()->name());
    }

    auto res = graph.save(algorithm);
    for (auto& element : res.elements) {
        if (element.first.startsWith("p")) {
            auto position = this->getPosition(element.first.mid(1).toInt());
            position->setPos(element.second);
        }
        else {
            auto transition = this->getTransition(element.first.mid(1).toInt());
            transition->setPos(element.second);
        }
    }

}
