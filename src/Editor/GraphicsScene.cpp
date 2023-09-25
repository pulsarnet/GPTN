//
// Created by Николай Муравьев on 13.01.2022.
//


#include <QMenu>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonDocument>
#include <QUndoStack>
#include <QPixmapCache>
#include "Commands.h"
#include "GraphicsScene.h"
#include "elements/PetriObject.h"
#include "elements/Position.h"
#include "elements/Transition.h"
#include "elements/ArrowLine.h"
#include "../Core/graphviz/GraphvizWrapper.h"
#include "GraphicsSceneActions.h"

GraphicsScene::GraphicsScene(ffi::PetriNet *net, QObject *parent)
    : QGraphicsScene(parent)
    , m_mode(Mode::A_Nothing)
    , m_allowMods(Mode::A_Nothing)
    , m_net(net)
    , m_undoStack(new QUndoStack(this))
{
    setSceneRect(-12500, -12500, 25000, 25000);
    setBackgroundBrush(Qt::white);

    auto positions = m_net->positions();
    auto transitions = m_net->transitions();
    auto connections = m_net->connections();

    for (auto position : positions) {
        addItem(new Position(QPointF(0, 0), m_net, position->index()));
    }

    for (auto transition : transitions) {
        addItem(new Transition(QPointF(0, 0), m_net, transition->index()));
    }

    for (auto connection : connections) {
        auto from = connection->from();
        auto to = connection->to();

        if (from.type == ffi::VertexType::Position) {
            auto position = getPosition((int)from.id);
            auto transition = getTransition((int)to.id);
            auto connectionLine = new ArrowLine(position, transition);
            connectionLine->createInNet(false);
            addItem(connectionLine);
        }
        else {
            auto transition = getTransition((int)from.id);
            auto position = getPosition((int)to.id);
            auto connectionLine = new ArrowLine(transition, position);
            connectionLine->createInNet(false);
            addItem(connectionLine);
        }
    }

    dotVisualization((char*)"dot");

    connect(this, &QGraphicsScene::selectionChanged, this, &GraphicsScene::onSelectionChanged);
    connect(m_undoStack, &QUndoStack::indexChanged, this, &GraphicsScene::onSceneChanged);
}


// todo разобраться
void GraphicsScene::setMode(Mode mod) {
    if (m_allowMods & mod) m_mode = mod;
    else qDebug() << "Mode " << mod << " not allowed!";
}

void GraphicsScene::setAllowMods(Modes mods) {
    m_allowMods = mods;
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
    else if (auto connection_line = dynamic_cast<ArrowLine*>(item); connection_line) {
        m_undoStack->push(ConnectCommand::disconnect(connection_line, this));
    }
}

void GraphicsScene::connectionStart(QGraphicsSceneMouseEvent *event) {
    auto item = itemAt(event->scenePos(), QTransform());
    if (auto petri = dynamic_cast<PetriObject*>(item); petri) {
        auto connection = new ArrowLine(petri, QLineF(item->scenePos(), event->scenePos()));
        addItem(connection);
        m_currentConnection = connection;
    }
}

void GraphicsScene::connectionCommit(QGraphicsSceneMouseEvent *event) {
    if (!m_currentConnection) return;

    auto item = netItemAt(event->scenePos());
    if (auto petri = dynamic_cast<PetriObject*>(item); petri) {
        if (m_currentConnection->from()->allowConnection(item)) {
            auto existing = getConnection(m_currentConnection->from(), petri);
            if (existing) {
                if (existing->from() == petri) {
                    if (existing->isBidirectional()) {
                        m_undoStack->push(ConnectCommand::setWeight(existing,
                                                                    (int)existing->netItem(true)->weight() + 1,
                                                                    true,
                                                                    this));
                    } else {
                        m_undoStack->push(ConnectCommand::setBidirectional(existing, this));
                    }

                } else {
                    m_undoStack->push(ConnectCommand::setWeight(existing,
                                                                (int)existing->netItem()->weight() + 1,
                                                                false,
                                                                this));
                }
            } else {
                // create new connection
                m_undoStack->push(ConnectCommand::connect(m_currentConnection->from(), petri, this));
            }
        }
    }

    connectionRollback(nullptr);
}

void GraphicsScene::connectionRollback(QGraphicsSceneMouseEvent *event) {
    Q_UNUSED(event)
    if (m_currentConnection) {
        removeItem(m_currentConnection);
        delete m_currentConnection; // а точно ли нужен delete, а может и deleteLater()
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

ffi::PetriNet *GraphicsScene::net() {
    return m_net;
}

void GraphicsScene::onSceneChanged() {
    if (m_restore)
        return;
    emit sceneChanged();
}

void GraphicsScene::setSimulation(ffi::Simulation* simulation) {
    m_simulation = simulation;
}

ffi::Simulation* GraphicsScene::simulation() const {
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

        ffi::Vertex* vertex;
        if (parent > 0) vertex = m_net->add_position_with_parent(id, parent);
        else vertex = m_net->add_position_with(id);

        vertex->set_markers(markers);
        vertex->set_name(name.toUtf8().data());

        addItem(new Position(pos, m_net, vertex->index()));
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

        addItem(new Transition(pos, m_net, vertex->index()));
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

        auto existing = getConnection(from, to);
        if (existing) {
            net()->connect(existing->to()->vertex(), existing->from()->vertex());
            existing->setBidirectional(true);
        } else {
            addItem(new ArrowLine(from, to, new ArrowLine::ConnectionState{1}));
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

void GraphicsScene::setConnectionWeight(ArrowLine *connection, int weight, bool reverse) {
    connection->netItem(reverse)->setWeight(weight);
    connection->updateConnection();
}

ArrowLine* GraphicsScene::getConnection(PetriObject *from, PetriObject *to) {
    auto it = std::find_if(
            m_connections.begin(),
            m_connections.end(),
            [&](ArrowLine* connection) {
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

    auto res = graph.save(algorithm);
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
    QPen pen(QColor(0, 0, 0, 150), 1, Qt::DotLine);
    painter->setPen(pen);
    painter->setOpacity(0.5);
    painter->setRenderHint(QPainter::Antialiasing);

    qreal left = int(rect.left()) - (int(rect.left()) % gridSize);
    qreal top = int(rect.top()) - (int(rect.top()) % gridSize);

    QVector<QLineF> lines;
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

    painter->drawLines(lines);
}

void GraphicsScene::registerItem(QGraphicsItem *item) {
    if (auto position = dynamic_cast<Position*>(item); position) {
        m_positions.push_back(position);
    } else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        m_transition.push_back(transition);
    } else if (auto connection = dynamic_cast<ArrowLine*>(item); connection) {
        m_connections.push_back(connection);
    }
}

void GraphicsScene::unregisterItem(QGraphicsItem *item) {
    if (auto position = dynamic_cast<Position*>(item); position) {
        m_positions.remove(m_positions.indexOf(position));
    } else if (auto transition = dynamic_cast<Transition*>(item); transition) {
        m_transition.remove(m_transition.indexOf(transition));
    } else if (auto connection = dynamic_cast<ArrowLine*>(item); connection) {
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
