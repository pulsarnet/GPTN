//
// Created by Николай Муравьев on 13.01.2022.
//


#include <QMenu>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include "../ffi/rust.h"
#include "graphic_scene.h"
#include "../elements/petri_object.h"
#include "../elements/position.h"
#include "../elements/transition.h"
#include "../elements/arrow_line.h"
#include "../graphviz/graphviz_wrapper.h"

GraphicScene::GraphicScene(ffi::PetriNet *net, QObject *parent) :
    m_mod(Mode::A_Nothing),
    m_allowMods(Mode::A_Nothing),
    m_net(net)
{
    setSceneRect(-12500, -12500, 25000, 25000);
    GraphVizWrapper graph;
    QString algorithm = "neato";

    auto positions = m_net->positions();
    auto transitions = m_net->transitions();
    auto connections = m_net->connections();

    for (int i = 0; i < positions.size(); i++) {
        graph.addCircle(QString("p%1").arg(positions[i]->index().id).toLocal8Bit().data(), QSizeF(80, 80));
        m_positions.push_back(new Position(QPointF(0, 0), m_net, positions[i]->index()));
        addItem(m_positions.last());
    }

    for (int i = 0; i < transitions.size(); i++) {
        graph.addRectangle(QString("t%1").arg(transitions[i]->index().id).toLocal8Bit().data(), QSizeF(120, 60));
        m_transition.push_back(new Transition(QPointF(0, 0), m_net, transitions[i]->index()));
        addItem(m_transition.last());
    }

    for (int i = 0; i < connections.size(); i++) {
        auto from = connections[i]->from();
        auto to = connections[i]->to();

        if (from.type == ffi::VertexType::Position) {
            auto position = getPosition(from.id);
            auto transition = getTransition(to.id);
            graph.addEdge(position->name(), transition->name());
            connectItems(position, transition, true);
        }
        else {
            auto transition = getTransition(from.id);
            auto position = getPosition(to.id);
            graph.addEdge(position->name(), transition->name());
            connectItems(transition, position, true);
        }
    }

    auto result = graph.save(algorithm.isEmpty() ? (char*)"sfdp" : algorithm.toLocal8Bit().data());
    for (auto& element : result.elements) {
        auto vertex = getVertex(element.first);
        vertex->setPos(element.second);
    }
}


void GraphicScene::setMode(Mode mod) {
    if (m_allowMods & mod) m_mod = mod;
    else qDebug() << "Mode " << mod << " not allowed!";
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
                m_dragInProgress = true;
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

            if (m_dragInProgress && !mouseGrabberItem()) {
                m_dragInProgress = false;
            }

            break;
        default:
            break;
    }

    //QGraphicsScene::mouseMoveEvent(event);
}

void GraphicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {

    if (m_dragInProgress) {
        m_dragInProgress = false;
        onSceneChanged();
    }

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
        onSceneChanged();
    }
    else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        auto index = m_transition.indexOf(transition);
        m_transition.removeAt(index);
        removeConnectionsAssociatedWith(transition);
        m_net->remove_transition(transition->vertex());
        delete transition;
        onSceneChanged();
    }
    else if (auto connection_line = dynamic_cast<ArrowLine*>(item); connection_line) {
        connection_line->disconnect(m_net);
        removeItem(connection_line);
        m_connections.removeAt(m_connections.indexOf(connection_line));
        delete connection_line;
        onSceneChanged();
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

void GraphicScene::onSceneChanged() {
    if (m_restore)
        return;

    emit sceneChanged();
}

QJsonDocument GraphicScene::json() const {
    auto n_positions = positions();
    auto n_transitions = transitions();
    auto n_connections = connections();

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

bool GraphicScene::fromJson(const QJsonDocument& document) {
    auto main = document.object();

    QJsonArray positions;
    QJsonArray transitions;
    QJsonArray connections;

    if (auto it = main.find("positions"); it != main.end()) {
        positions = it->toArray();
    } else {
        return false;
    }

    if (auto it = main.find("transitions"); it != main.end()) {
        transitions = it->toArray();
    } else {
        return false;
    }

    if (auto it = main.find("connections"); it != main.end()) {
        connections = it->toArray();
    } else {
        return false;
    }

    // Reset state
    this->removeAll();

    for (auto position : positions) {
        auto obj = position.toObject();

        auto id = obj.value("id").toInt();
        auto markers = obj.value("markers").toInt();
        auto parent = obj.value("parent").toInt();
        auto name = obj.value("name").toString();

        QJsonObject point = obj.value("pos").toObject();
        auto pos = QPointF(point.value("x").toDouble(), point.value("y").toDouble());

        ffi::Vertex* vertex;
        if (parent > 0) vertex = m_net->add_position_with_parent(id, parent);
        else vertex = m_net->add_position_with(id);

        vertex->set_markers(markers);
        vertex->set_name(name.toUtf8().data());

        addPosition(vertex, pos);
    }

    for (auto transition : transitions) {
        auto obj = transition.toObject();

        auto id = obj.value("id").toInt();
        auto parent = obj.value("parent").toInt();
        auto name = obj.value("name").toString();

        QJsonObject point = obj.value("pos").toObject();
        auto pos = QPointF(point.value("x").toDouble(), point.value("y").toDouble());

        ffi::Vertex* vertex;
        if (parent > 0) vertex = m_net->add_transition_with_parent(id, parent);
        else vertex = m_net->add_transition_with(id);

        vertex->set_name(name.toUtf8().data());

        addTransition(vertex, pos);
    }

    for (auto connection : connections) {
        auto obj = connection.toObject();

        auto fromObj = obj.value("from").toObject();
        auto fromType = (ffi::VertexType)fromObj.value("type").toInt();
        auto fromId = fromObj.value("index").toInt();

        auto toObj = obj.value("to").toObject();
        auto toType = (ffi::VertexType)toObj.value("type").toInt();
        auto toId = toObj.value("index").toInt();

        PetriObject *from, *to;
        if (fromType == ffi::VertexType::Position) from = getPosition(fromId);
        else from = getTransition(fromId);

        if (toType == ffi::VertexType::Position) to = getPosition(toId);
        else to = getTransition(toId);

        connectItems(from, to);
    }

    return true;
}

void GraphicScene::removeAll() {
    QGraphicsScene::clear();

    // TODO: Удаление сети
    m_net = ffi::PetriNet::create(); // TODO: Надо удалить
    m_transition.clear();
    m_positions.clear();
    m_connections.clear();
}

Position* GraphicScene::addPosition(int index, const QPointF &point) {
    return addPosition(index == -1 ? m_net->add_position() : m_net->add_position_with(index), point);
}

Position *GraphicScene::addPosition(ffi::Vertex *position, const QPointF &point) {
    auto pos = new Position(point, m_net, position->index());
    m_positions.push_back(pos);
    addItem(pos);

    onSceneChanged();
    return pos;
}

Transition* GraphicScene::addTransition(int index, const QPointF &point) {
    return addTransition(index == -1 ? m_net->add_transition() : m_net->add_transition_with(index), point);
}

Transition *GraphicScene::addTransition(ffi::Vertex *transition, const QPointF &point) {
    auto tran = new Transition(point, m_net, transition->index());
    m_transition.push_back(tran);
    addItem(tran);

    onSceneChanged();

    return tran;
}


ArrowLine* GraphicScene::connectItems(PetriObject *from, PetriObject *to, bool no_add) {

    auto arrowLine = new ArrowLine(from, QLineF(from->connectionPos(to, false), to->connectionPos(from, true)));
    arrowLine->setTo(to);

    if (!no_add)
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
