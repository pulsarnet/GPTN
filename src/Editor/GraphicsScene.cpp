#include <QMenu>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include "Commands.h"
#include "GraphicsScene.h"
#include "elements/PetriObject.h"
#include "elements/Position.h"
#include "elements/Transition.h"
#include "elements/Edge.h"
#include "../Core/graphviz/GraphvizWrapper.h"
#include "GraphicsSceneActions.h"
#include <ptn/net.h>

#include "Elements/DirectedArc.h"
#include "Elements/InhibitorArc.h"

GraphicsScene::GraphicsScene(net::PetriNet *net, QObject *parent)
    : QGraphicsScene(parent)
    , m_mode(Mode::A_Nothing)
    , m_allowModes(Mode::A_Nothing)
    , m_net(net)
    , m_undoStack(new QUndoStack(this))
{
    setSceneRect(-12500, -12500, 25000, 25000);
    setBackgroundBrush(Qt::NoBrush);

    auto positions = m_net->positions();
    auto transitions = m_net->transitions();
    auto directed_arcs = m_net->directed_arcs();
    auto inhibitor_arcs = m_net->inhibitor_arcs();

    for (const auto position : positions) {
        addItem(new Position(QPointF(0, 0), m_net, position->index()));
    }

    for (const auto transition : transitions) {
        addItem(new Transition(QPointF(0, 0), m_net, transition->index()));
    }

    for (auto arc : directed_arcs) {
        auto from = arc->from();
        auto to = arc->to();

        if (from.t == vertex::VertexType::Position) {
            const auto position = getPosition((int)from.id);
            const auto transition = getTransition((int)to.id);
            const auto connectionLine = new DirectedArc(position, transition);
            connectionLine->blockChange(true);
            addItem(connectionLine);
            connectionLine->blockChange(false);
        } else {
            const auto transition = getTransition((int)from.id);
            const auto position = getPosition((int)to.id);
            const auto connectionLine = new DirectedArc(transition, position);
            connectionLine->blockChange(true);
            addItem(connectionLine);
            connectionLine->blockChange(false);
        }
    }

    for (auto arc : inhibitor_arcs) {
        auto place = arc->place();
        auto transition = arc->transition();

        const auto p = getPosition((int)place.id);
        const auto t = getTransition((int)transition.id);
        const auto connectionLine = new InhibitorArc(p, t);
        connectionLine->blockChange();
        addItem(connectionLine);
        connectionLine->blockChange(false);
    }

    dotVisualization((char*)"dot");

    connect(this, &QGraphicsScene::selectionChanged, this, &GraphicsScene::onSelectionChanged);
    connect(m_undoStack, &QUndoStack::indexChanged, this, &GraphicsScene::onSceneChanged);
}


void GraphicsScene::setMode(Mode mode) {
    if (m_allowModes & mode) {
        m_mode = mode;
        qDebug() << "Change scene mode to" << mode;
    } else {
        qDebug() << "Mode" << mode << "not allowed!";
    }
}

void GraphicsScene::setAllowMods(Modes mods) {
    m_allowModes = mods;
}

void GraphicsScene::onSelectionChanged() {
    auto selected = selectedItems();
    m_actions->hAlignmentAction()->setEnabled(!selected.empty());
    m_actions->vAlignmentAction()->setEnabled(!selected.empty());
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (m_simulation) return;

    if (event->button() == Qt::LeftButton) {
        switch (m_mode) {
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
                // Чтобы получить grabber item
                QGraphicsScene::mousePressEvent(event);

                if (auto dragged = dynamic_cast<PetriObject*>(mouseGrabberItem())) {
                    m_dragInProgress = true;
                    for (auto item : selectedItems()) {
                        if (auto petriItem = dynamic_cast<PetriObject*>(item)) {
                            m_draggedItems.push_back({petriItem, petriItem->scenePos()});
                        }
                    }
                }
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
    } else if (event->button() == Qt::RightButton) {
        if (m_mode == A_Connection) connectionRollback(event);
        else QGraphicsScene::mousePressEvent(event);
    }
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (m_simulation) return;

    switch (m_mode) {
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
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (m_simulation) return;

    if (m_dragInProgress) {
        if (!m_draggedItems.isEmpty()) {
            QList<MoveCommandData> data;
            for (auto& [m_draggedItem, m_dragItemPos] : m_draggedItems) {
                if (m_draggedItem->scenePos() != m_dragItemPos) {
                    data.push_back(MoveCommandData{m_draggedItem, m_dragItemPos, m_draggedItem->scenePos()});
                }
            }

            if (!data.isEmpty()) {
                m_undoStack->push(new MoveCommand(std::move(data)));
            }
        }

        m_dragInProgress = false;
        m_draggedItems.clear();
    }

    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::insertPosition(QGraphicsSceneMouseEvent *event) {
    auto position = new Position(event->scenePos(), m_net, m_net->add_position()->index());
    m_undoStack->push(new AddCommand(position, this));
}

void GraphicsScene::insertTransition(QGraphicsSceneMouseEvent *event) {
    auto transition = new Transition(event->scenePos(), m_net, m_net->add_transition()->index());
    m_undoStack->push(new AddCommand(transition, this));
}

void GraphicsScene::removeObject(QGraphicsSceneMouseEvent *event) {
    auto item = itemAt(event->scenePos(), QTransform());
    if (auto scenePetriObject = dynamic_cast<PetriObject*>(item); scenePetriObject) {
        m_undoStack->push(new RemoveCommand(scenePetriObject, this));
    }
    else if (auto connection_line = dynamic_cast<Edge*>(item); connection_line) {
        m_undoStack->push(ConnectCommand::disconnect(connection_line, this));
    }
}

void GraphicsScene::connectionStart(QGraphicsSceneMouseEvent *event) {
    auto item = itemAt(event->scenePos(), QTransform());
    if (auto petri = dynamic_cast<PetriObject*>(item); petri) {
        Edge* edge;
        if (m_edgeType == Direct) {
            edge = new DirectedArc(petri, QLineF(item->scenePos(), event->scenePos()));
        } else if (auto place = dynamic_cast<Position*>(petri); m_edgeType == Inhibitor && place) {
            edge = new InhibitorArc(place, QLineF(item->scenePos(), event->scenePos()));
        } else {
            return;
        }

        edge->blockChange(true);
        addItem(edge);
        m_currentConnection = edge;
    }
}

void GraphicsScene::connectionCommit(QGraphicsSceneMouseEvent *event) {
    if (!m_currentConnection) return;

    if (auto item = netItemAt(event->scenePos()); item) {
        if (m_currentConnection->allowDestination(item)) {
            if (auto existing = getConnection(m_currentConnection->from(), item); existing) {
                if (existing->typeId() != m_currentConnection->typeId()) {
                    qWarning() << "existing arc with other type " << existing->typeId();
                    connectionRollback(nullptr);
                    return;
                }

                // if supports bidirectional
                if (existing->flags() & Edge::EdgeCanBidirectional) {
                    if (existing->from() == item && !existing->isBidirectional()) {
                        m_undoStack->push(ConnectCommand::setBidirectional(existing, this));
                        connectionRollback(nullptr);
                        return;
                    }
                }

                // if supports weight
                if (existing->flags() & Edge::EdgeHasWeight) {
                    if (existing->from() == item) {
                        if (existing->isBidirectional()) {
                            m_undoStack->push(ConnectCommand::setWeight(existing,
                                                                    (int)existing->weight(true) + 1,
                                                                    true,
                                                                    this));
                        }
                    } else {
                        m_undoStack->push(ConnectCommand::setWeight(existing,
                                                                (int)existing->weight(false) + 1,
                                                                false,
                                                                this));
                    }
                }
            } else {
                // create new connection
                if (m_edgeType == Direct)
                    m_undoStack->push(ConnectCommand::directed(m_currentConnection->from(), item, this));
                else
                    m_undoStack->push(ConnectCommand::inhibitor((Position*)m_currentConnection->from(), (Transition*)item, this));
            }
        }
    }

    connectionRollback(nullptr);
}

void GraphicsScene::connectionRollback(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    if (m_currentConnection) {
        removeItem(m_currentConnection);
        delete m_currentConnection;
        m_currentConnection = nullptr;
    }
}

PetriObject *GraphicsScene::netItemAt(const QPointF &pos) {
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

void GraphicsScene::rotateObject(QGraphicsSceneMouseEvent *event) {
    auto item = netItemAt(event->scenePos());
    if (item) {
        if (auto transition = dynamic_cast<Transition*>(item); transition) {
            m_undoStack->push(new RotateCommand(transition, 90));
        }
    }
}

net::PetriNet *GraphicsScene::net() {
    return m_net;
}

void GraphicsScene::onSceneChanged() {
    if (m_restore)
        return;
    emit sceneChanged();
}

void GraphicsScene::setSimulation(ptn::modules::simulation::Simulation* simulation) {
    m_simulation = simulation;
}

ptn::modules::simulation::Simulation* GraphicsScene::simulation() const {
    return m_simulation;
}

QJsonDocument GraphicsScene::json() const {
    auto n_positions = positions();
    auto n_transitions = transitions();
    auto n_connections = connections();

    QJsonArray positions;
    for (auto & n_position : n_positions) {
        auto net_position = n_position->vertex();

        QJsonObject position;
        position.insert("id", QJsonValue((int)net_position->index().id));
        position.insert("markers", QJsonValue((int)net_position->markers()));
        position.insert("parent", QJsonValue((int)net_position->parent().id));

        // todo name
        //position.insert("name", QJsonValue(net_position->get_name(false)));

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
        transition.insert("parent", QJsonValue((int)net_transition->parent().id));

        //todo name
        //transition.insert("name", QJsonValue(net_transition->get_name(false)));

        QPointF pos = n_transition->center();
        QJsonObject point;
        point.insert("x", pos.x());
        point.insert("y", pos.y());
        transition.insert("pos", point);

        transitions.push_back(transition);
    }

    // TODO: directed
    QJsonArray connections;
    for (auto & n_connection : n_connections) {
        auto connection_from = n_connection->from();
        auto connection_to = n_connection->to();

        QJsonObject connection;
        QJsonObject from;
        from.insert("type", (int)connection_from->vertex()->index().t);
        from.insert("index", (int)connection_from->vertex()->index().id);

        QJsonObject to;
        to.insert("type", (int)connection_to->vertex()->index().t);
        to.insert("index", (int)connection_to->vertex()->index().id);

        connection.insert("from", from);
        connection.insert("to", to);

        connections.push_back(connection);

        // TODO: save & restore weight
        if (n_connection->isBidirectional()) {
            connection.insert("from", to);
            connection.insert("to", from);
            connections.push_back(connection);
        }
    }

    QJsonObject main;
    main.insert("positions", positions);
    main.insert("transitions", transitions);
    main.insert("connections", connections);

    return QJsonDocument(main);
}

bool GraphicsScene::fromJson(const QJsonDocument& document) {
    auto main = document.object();

    QJsonArray positions;
    QJsonArray transitions;
    QJsonArray connections;

    if (auto it = main.find("positions"); it != main.end()) {
        positions = it->toArray();
    }

    if (auto it = main.find("transitions"); it != main.end()) {
        transitions = it->toArray();
    }

    if (auto it = main.find("connections"); it != main.end()) {
        connections = it->toArray();
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

        vertex::Vertex* vertex;
        vertex = m_net->insert_position(id, parent);

        vertex->set_markers(markers);
        vertex->set_label(name.toUtf8().data());

        addItem(new Position(pos, m_net, vertex->index()));
    }

    for (auto transition : transitions) {
        auto obj = transition.toObject();

        auto id = obj.value("id").toInt();
        auto parent = obj.value("parent").toInt();
        auto name = obj.value("name").toString();

        QJsonObject point = obj.value("pos").toObject();
        auto pos = QPointF(point.value("x").toDouble(), point.value("y").toDouble());

        vertex::Vertex* vertex;
        vertex = m_net->insert_transition(id, parent);
        vertex->set_label(name.toUtf8().data());

        addItem(new Transition(pos, m_net, vertex->index()));
    }

    for (auto connection : connections) {
        auto obj = connection.toObject();

        auto fromObj = obj.value("from").toObject();
        auto fromType = (vertex::VertexType)fromObj.value("type").toInt();
        auto fromId = fromObj.value("index").toInt();

        auto toObj = obj.value("to").toObject();
        auto toType = (vertex::VertexType)toObj.value("type").toInt();
        auto toId = toObj.value("index").toInt();

        PetriObject *from, *to;
        if (fromType == vertex::VertexType::Position) from = getPosition(fromId);
        else from = getTransition(fromId);

        if (toType == vertex::VertexType::Position) to = getPosition(toId);
        else to = getTransition(toId);

        auto existing = getConnection(from, to);
        if (existing) {
            net()->add_directed(existing->to()->vertexIndex(), existing->from()->vertexIndex());
            existing->setBidirectional(true);
        } else {
            addItem(new DirectedArc(from, to));
        }
    }

    return true;
}

void GraphicsScene::removeAll() {
    QGraphicsScene::clear();

    m_net->clear();
    m_transition.clear();
    m_positions.clear();
    m_connections.clear();
}


Edge* GraphicsScene::getConnection(const PetriObject *from, const PetriObject *to) {
    auto it = std::find_if(
            m_connections.begin(),
            m_connections.end(),
            [&](Edge* connection) {
                return (connection->from() == from && connection->to() == to)
                       || (connection->from() == to && connection->to() == from);
            });

    return it != m_connections.end() ? *it : nullptr;
}

void GraphicsScene::slotHorizontalAlignment(bool triggered) {
    Q_UNUSED(triggered)

    qreal y = 0;
    int elements = 0;

    for (auto item : selectedItems()) {
        if (dynamic_cast<PetriObject*>(item)) {
            y += item->pos().y();
            elements++;
        }
    }

    QList<MoveCommandData> data;
    if (elements > 0) {
        y /= (qreal)elements;
        for (auto item : selectedItems()) {
            if (auto cast = dynamic_cast<PetriObject*>(item); cast) {
                QPointF newPos(cast->pos().x(), y);
                if (newPos != cast->pos()) {
                    data.push_back({cast, cast->pos(), newPos});
                }
            }
        }
    }

    if (!data.isEmpty()) {
        m_undoStack->push(new MoveCommand(std::move(data)));
    }
}

void GraphicsScene::slotVerticalAlignment(bool triggered) {
    Q_UNUSED(triggered)

    qreal x = 0;
    int elements = 0;

    for (auto item : selectedItems()) {
        if (dynamic_cast<PetriObject*>(item)) {
            x += item->pos().x();
            elements++;
        }
    }

    QList<MoveCommandData> data;
    if (elements > 0) {
        x /= (qreal)elements;
        for (auto item : selectedItems()) {
            if (auto cast = dynamic_cast<PetriObject*>(item); cast) {
                QPointF newPos(x, cast->pos().y());
                if (newPos != cast->pos()) {
                    data.push_back({cast, cast->pos(), newPos});
                }
            }
        }
    }

    if (!data.isEmpty()) {
        m_undoStack->push(new MoveCommand(std::move(data)));
    }

}

Transition *GraphicsScene::getTransition(int index) {
    auto it = std::find_if(m_transition.begin(), m_transition.end(), [=](Transition* t) {
        return t->index() == index;
    });

    return it == m_transition.end() ? nullptr : *it;
}

Position *GraphicsScene::getPosition(int index) {
    auto it = std::find_if(m_positions.begin(), m_positions.end(), [=](Position* p) {
        return p->index() == index;
    });
    return it == m_positions.end() ? nullptr : *it;
}

void GraphicsScene::markPosition(QGraphicsSceneMouseEvent *event) {
    auto item = netItemAt(event->scenePos());
    if (auto position = dynamic_cast<Position*>(item); position) {
        if (event->buttons() | Qt::LeftButton) {
            bool sub = event->modifiers() & Qt::Modifier::SHIFT;
            if (!sub || position->markers() > 0) {
                m_undoStack->push(new MarkCommand(position,!sub,this));
            }
        }
    }
}

void GraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    if (m_mode == A_Move) QGraphicsScene::mouseDoubleClickEvent(event);
    else if (m_mode == A_Marker) markPosition(event);
}

void GraphicsScene::dotVisualization(char* algorithm) {
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

    auto res = graph.build(algorithm);
    QList<MoveCommandData> moveData;
    for (auto& element : res.elements) {
        if (element.first.startsWith("p")) {
            auto position = this->getPosition(element.first.mid(1).toInt());
            if (position->pos() != element.second) {
                moveData.push_back({position, position->pos(), element.second});
            }
        }
        else {
            auto transition = this->getTransition(element.first.mid(1).toInt());
            if (transition->pos() != element.second) {
                moveData.push_back({transition, transition->pos(), element.second});
            }
        }
    }

    if (!moveData.isEmpty())
        m_undoStack->push(new MoveCommand(std::move(moveData)));
}

void GraphicsScene::drawBackground(QPainter *painter, const QRectF &rect) {
    const int gridSize = 50.;
    QGraphicsScene::drawBackground(painter, rect);

    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVector<QLineF> lines;
    lines.reserve(100);
    qreal x = left;
    while(x <= rect.right()) {
        lines.append(QLineF(
                QPointF(x, rect.top()),
                QPointF(x, rect.bottom())
        ));

        x += gridSize;
    }

    qreal y = top;
    while (y <= rect.bottom()) {
        lines.append(QLineF(
                QPointF(rect.left(), y),
                QPointF(rect.right(), y)
        ));

        y += gridSize;
    }

    painter->setPen(QPen(Qt::lightGray, 0));
    painter->drawLines(lines);
}

void GraphicsScene::registerItem(QGraphicsItem *item) {
    if (auto position = dynamic_cast<Position*>(item); position) {
        m_positions.push_back(position);
    } else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        m_transition.push_back(transition);
    } else if (auto connection = dynamic_cast<Edge*>(item); connection) {
        m_connections.push_back(connection);
    }
}

void GraphicsScene::unregisterItem(QGraphicsItem *item) {
    if (auto position = dynamic_cast<Position*>(item); position) {
        m_positions.remove(m_positions.indexOf(position));
    } else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        m_transition.remove(m_transition.indexOf(transition));
    } else if (auto connection = dynamic_cast<Edge*>(item); connection) {
        m_connections.remove(m_connections.indexOf(connection));
    }
}

GraphicsSceneActions *GraphicsScene::actions() {
    if (!m_actions)
        createActions();

    return m_actions;
}

void GraphicsScene::createActions() {
    m_actions = new GraphicsSceneActions(this);
}
