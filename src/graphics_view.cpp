//
// Created by nmuravev on 12/13/2021.
//

#include <QFile>
#include <QSettings>
#include "../include/graphics_view.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {
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

void GraphicsView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        shift_pressed = true;
    }
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        shift_pressed = false;
    }
}

void GraphicsView::updateConnections() {
    for (int i = 0; i < connections.size(); i++) {
        connections[i].first->setLine(connectObjects(
                dynamic_cast<PetriObject *>(connections[i].second.first),
                dynamic_cast<PetriObject *>(connections[i].second.second)));
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {

    auto mapToScenePos = this->mapToScene(event->pos());
    if (event->button() == Qt::LeftButton) {
        if (action == Action::A_Position) {
            items.push_back(new Position);
            items.back()->setPos(mapToScenePos);
            scene->addItem(items.back());
        }
        else if (action == Action::A_Transition) {
            items.push_back(new Transition(mapToScenePos));
            scene->addItem(items.back());
        }
        else if (action == Action::A_Rotate) {
            if (auto item = dynamic_cast<Transition*>(scene->itemAt(mapToScenePos, transform())); item) {
                item->setRotation(item->rotation() == 90 ? 0 : 90);
                updateConnections();
            }
        }
        else if (auto item = scene->itemAt(mapToScenePos, transform()); item && action == Action::A_Connect) {
            auto line_item = new ArrowLine(QLineF(item->scenePos().x(), item->scenePos().y(), mapToScenePos.x(), mapToScenePos.y()));
            scene->addItem(line_item);
            connections.push_back(std::make_pair(line_item, std::make_pair(item, nullptr)));
            current_connection = &connections.back();
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
        auto line = current_connection->first->line();
        line.setP2(this->mapToScene(event->pos()));
        current_connection->first->setLine(line);
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

QGraphicsItem *GraphicsView::itemAt(QPointF pos) {
    for (auto item: items) {
        if (item->sceneBoundingRect().contains(pos)) return item;
    }
    return nullptr;
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {

    setCursor(QCursor(Qt::ArrowCursor));
    setInteractive(true);

    if (auto item = itemAt(this->mapToScene(event->pos())); item && current_connection) {
        if ((item != current_connection->second.first) &&
            ((dynamic_cast<Position*>(current_connection->second.first) && dynamic_cast<Transition*>(item))
             || (dynamic_cast<Transition*>(current_connection->second.first) && dynamic_cast<Position*>(item))))
        {
            current_connection->first->setLine(connectObjects(
                    dynamic_cast<PetriObject *>(current_connection->second.first),
                    dynamic_cast<PetriObject *>(item)));

            current_connection->second.second = item;
            current_connection = nullptr;
        }
    }

    if (current_connection && current_connection->second.second == nullptr) {
        for (int i = 0; i < connections.size(); i++) {
            if (&connections[i] == current_connection) {
                connections.removeAt(i);
                scene->removeItem(current_connection->first);
                break;
            }
        }
        current_connection = nullptr;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

QLineF GraphicsView::connectObjects(PetriObject *from, PetriObject *to) {
    qreal from_angle = from->angleBetween(to->center());
    qreal to_angle = to->angleBetween(from->center());

    QPointF pointFrom = from->connectionPos(from_angle);
    QPointF pointTo = to->connectionPos(to_angle);

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
        data["id"] = QVariant("id_1");

        if (dynamic_cast<Transition*>(item)) {
            data["type"] = QVariant(tr("transition"));
        }
        else if (dynamic_cast<Position*>(item)) {
            data["type"] = QVariant(tr("position"));
        }

        data_list << data;
    }

    QVariantList connections_list;
    foreach(Connection conn, this->connections) {
        QVariantHash data;
        // TODO: Изменить на идентификаторы
        data["from"] = QVariant(conn.second.first->pos());
        data["to"] = QVariant(conn.second.second->pos());

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
            if (hash["type"] == "position") {
                object = new Position;
                object->setPos(hash["pos"].toPointF());
                object->setRotation(hash["transition"].toDouble());
            }
            else if (hash["type"] == "transition") {
                object = new Transition(hash["pos"].toPointF());
                object->setRotation(hash["rotation"].toDouble());
            }

            this->items.push_back(object);
            this->scene->addItem(object);
        }
    }

}