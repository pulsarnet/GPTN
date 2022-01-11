//
// Created by nmuravev on 12/13/2021.
//

#include <QFile>
#include <QSettings>
#include "../../include/view/graphics_view.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {

    m_net = PetriNet::make();

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setMinimumHeight(100);
    this->setMinimumWidth(100);

    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);

    scene = new QGraphicsScene();
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->setScene(scene);

    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    zoom = new GraphicsViewZoom(this);
    zoom->set_modifier(Qt::NoModifier);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    connect(scene, &QGraphicsScene::changed, this, &GraphicsView::slotSceneChanged);
}

void GraphicsView::setAction(GraphicsView::Action action) {
    this->action = action;
}

void GraphicsView::updateConnections() {
    std::for_each(connections.begin(), connections.end(), [&](ArrowLine* connection) {
        connection->setLine(connectObjects(connection->from(), connection->to()));
    });
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {

    auto mapToScenePos = this->mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) {
        if (action == Action::A_Position) {
            items.push_back(new Position(mapToScenePos, m_net->add_position()));
            scene->addItem(items.back());
        }
        else if (action == Action::A_Transition) {
            items.push_back(new Transition(mapToScenePos, m_net->add_transition()));
            scene->addItem(items.back());
        }
        else if (action == Action::A_Rotate) {
            if (auto item = dynamic_cast<Transition*>(scene->itemAt(mapToScenePos, transform())); item) {
                item->setRotation(item->rotation() == 90 ? 0 : 90);
                updateConnections();
            }
        }
        else if (auto item = scene->itemAt(mapToScenePos, transform()); item) {
            if (action == Action::A_Connect) {
                auto petri = dynamic_cast<PetriObject*>(item);
                auto line_item = new ArrowLine(petri, QLineF(item->scenePos().x(), item->scenePos().y(), mapToScenePos.x(), mapToScenePos.y()));
                scene->addItem(line_item);
                connections.push_back(line_item);
                current_connection = line_item;
            }
            else if (action == Action::A_Remove) {
                scene->removeItem(item);

                if (auto petriObject = PetriObject::castTo<PetriObject>(item); petriObject) {
                    QMutableListIterator<ArrowLine*> connection_mut(connections);
                    while (connection_mut.hasNext()) {
                        auto next = connection_mut.next();
                        if (next->from() == petriObject || next->to() == petriObject) {
                            next->disconnect(m_net);
                            scene->removeItem(next);
                            connection_mut.remove();
                            delete next;
                        }
                    }

                    QMutableListIterator<QGraphicsItem*> items_mut(items);
                    while (items_mut.hasNext()) {
                        auto next = items_mut.next();
                        if (next == item) {
                            items_mut.remove();
                        }
                    }

                    m_net->remove_object(petriObject);
                }
                else if (auto connection = dynamic_cast<ArrowLine*>(item); connection) {
                    connection->from()->connectTo(m_net, connection->to());
                    connection->disconnect(m_net);
                    QMutableListIterator<ArrowLine*> connection_mut(connections);
                    while (connection_mut.hasNext()) {
                        auto next = connection_mut.next();
                        if (next == connection) {
                            connection_mut.remove();
                        }
                    }
                }

                delete item;

                emit signalRemoveItem();
            }
        }
    }
    else if (event->button() == Qt::MiddleButton) {
        m_origin = event->pos();
        setCursor(QCursor(Qt::SizeAllCursor));
        setInteractive(false);
    }

    if (action == Action::A_Move) QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {

    if (event->buttons() & Qt::LeftButton && action == Action::A_Connect && current_connection) {
        auto line = current_connection->line();
        line.setP2(this->mapToScene(event->pos()));
        current_connection->setLine(line);
    }
    else if (event->buttons() & Qt::MiddleButton) {
        QPointF oldp = mapToScene(m_origin.toPoint());
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        m_origin = event->pos();
    }
    else updateConnections();

    if (action == Action::A_Move) QGraphicsView::mouseMoveEvent(event);
}

PetriObject *GraphicsView::itemAt(QPointF pos) {
    for (auto item: items) {
        if (item->sceneBoundingRect().contains(pos) && dynamic_cast<PetriObject*>(item)) return PetriObject::castTo<PetriObject>(item);
    }
    return nullptr;
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {

    setCursor(QCursor(Qt::ArrowCursor));
    setInteractive(true);

    if (auto item = itemAt(this->mapToScene(event->pos())); item && current_connection) {
        if (current_connection->from()->allowConnection(item))
        {
            current_connection->setLine(connectObjects(current_connection->from(), item));
            current_connection->setTo(item);
            current_connection->from()->connectTo(m_net, current_connection->to());
            current_connection = nullptr;
        }
    }

    if (current_connection && current_connection->to() == nullptr) {
        for (int i = 0; i < connections.size(); i++) {
            if (connections[i] == current_connection) {
                connections.removeAt(i);
                scene->removeItem(current_connection);
                break;
            }
        }
        current_connection = nullptr;
    }

    if (this->action == A_Marker) {
        if (auto item = itemAt(this->mapToScene(event->pos())); item && item->objectType() == PetriObject::Position) {
            auto position = PetriObject::castTo<Position>(item);
            if (event->button() == Qt::LeftButton) position->add_marker();
            else if (event->button() == Qt::RightButton) position->remove_marker();
            update();
        }
    }

    QGraphicsView::mouseReleaseEvent(event);
}

QLineF GraphicsView::connectObjects(PetriObject *from, PetriObject *to) {

    QPointF pointFrom = from->connectionPos(to, true);
    QPointF pointTo = to->connectionPos(from, false);

    return {pointFrom, pointTo};
}

void GraphicsView::resizeEvent(QResizeEvent *event) {
    scene->setSceneRect(0, 0, 65535, 65535);
    QGraphicsView::resizeEvent(event);
}

GraphicsView::Action GraphicsView::currentAction() {
    return action;
}

void GraphicsView::slotSceneChanged(const QList<QRectF>& region) {
    Q_UNUSED(region);
    emit signalSceneChanged();
}

void GraphicsView::saveToFile(QFile &file) {

    QVariantList data_list;
    foreach(QGraphicsItem* item, this->items) {
        QVariantHash data;
        data["pos"] = QVariant(item->pos());
        data["rotation"] = QVariant(item->rotation());

        if (auto transition = dynamic_cast<Transition*>(item); transition) {
            data["type"] = QVariant(tr("transition"));
            data["id"] = QVariant(transition->index());
        }
        else if (auto position = dynamic_cast<Position*>(item); position) {
            data["type"] = QVariant(tr("position"));
            data["id"] = QVariant(position->index());
        }

        data_list << data;
    }

    QVariantList connections_list;
    foreach(ArrowLine* conn, this->connections) {
        QVariantHash from;
        from["type"] = QVariant(conn->from()->objectTypeStr());
        from["id"] = QVariant(conn->from()->index());

        QVariantHash to;
        to["type"] = QVariant(conn->to()->objectTypeStr());
        to["id"] = QVariant(conn->to()->index());

        QVariantHash data;
        data["from"] = QVariant(from);
        data["to"] = QVariant(to);

        connections_list << data;
    }

    QVariantHash common_data;
    common_data["items"] = data_list;
    common_data["connections"] = connections_list;

    QVariant data(common_data);

    QDataStream stream(&file);
    stream << data;

}

void GraphicsView::openFile(QFile &file) {

    QDataStream stream(&file);
    QVariant data;

    stream >> data;

    qDebug() << data;

    auto common_data = data.toHash();

    if (common_data.contains("items")) {
        foreach(QVariant data, common_data.value("items").toList()) {
            QVariantHash hash = data.toHash();
            PetriObject* object = nullptr;
            qDebug() << hash["id"].toInt();
            if (hash["type"] == "position") {
                object = new Position(hash["pos"].toPointF(), this->m_net->add_position_with(hash["id"].toInt()));
                object->setPos(hash["pos"].toPointF());
                object->setRotation(hash["transition"].toDouble());
            }
            else if (hash["type"] == "transition") {
                object = new Transition(hash["pos"].toPointF(), this->m_net->add_transition_with(hash["id"].toInt()));
                object->setRotation(hash["rotation"].toDouble());
            }

            this->items.push_back(object);
            this->scene->addItem(object);
        }
    }

    if (common_data.contains("connections")) {
        foreach(QVariant connection, common_data.value("connections").toList()) {
            QVariantHash hash = connection.toHash();

            QVariantHash from = hash["from"].toHash();
            QVariantHash to = hash["to"].toHash();

            auto find_from = [=](QGraphicsItem* item) {
                auto object = dynamic_cast<PetriObject*>(item);
                return object->objectTypeStr() == from["type"].toString() && object->index() == from["id"].toInt();
            };

            auto find_to = [=](QGraphicsItem* item) {
                auto object = dynamic_cast<PetriObject*>(item);
                return object->objectTypeStr() == to["type"].toString() && object->index() == to["id"].toInt();
            };

            auto point1_it = std::find_if(this->items.begin(), this->items.end(), find_from);
            auto point2_it = std::find_if(this->items.begin(), this->items.end(), find_to);

            PetriObject* point1 = dynamic_cast<PetriObject*>(*point1_it);
            PetriObject* point2 = dynamic_cast<PetriObject*>(*point2_it);

            auto line_item = new ArrowLine(point1, QLineF(point1->scenePos().x(), point1->scenePos().y(), point2->scenePos().x(), point2->scenePos().y()));
            line_item->setTo(point2);
            scene->addItem(line_item);
            connections.push_back(line_item);
            point1->connectTo(m_net, point2);
        }

        updateConnections();
    }

}

PetriObject *GraphicsView::addPosition(QString &name, QPointF point) {

    auto index = name.mid(1).toInt();

    auto pos = new Position(point, m_net->add_position_with(index));
    items.push_back(pos);
    scene->addItem(pos);

    return pos;
}

PetriObject *GraphicsView::addTransition(QString &name, QPointF point) {
    auto index = name.mid(1).toInt();

    auto tran = new Transition(point, m_net->add_transition_with(index));
    items.push_back(tran);
    scene->addItem(tran);

    return tran;
}

void GraphicsView::newConnection(PetriObject *from, PetriObject *to) {
    auto conn_line = new ArrowLine(from, connectObjects(from, to));
    conn_line->setTo(to);

    scene->addItem(conn_line);

    conn_line->from()->connectTo(m_net, conn_line->to());

    connections.push_back(conn_line);
}