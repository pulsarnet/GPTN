//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_GRAPHICS_VIEW_H
#define FFI_RUST_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
#include <QTransform>
#include "graphics_view_zoom.h"
#include "position.h"
#include "arrow_line.h"

class GraphicsView : public QGraphicsView {

public:
    enum Action {
        A_Position,
        A_Transition,
        A_Connect,
        A_Move,
        A_Rotate,
        A_Nothing
    };

public:

    GraphicsView(QWidget* parent = nullptr): QGraphicsView(parent) {
        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        this->setMinimumHeight(100);
        this->setMinimumWidth(100);

        this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        this->setWindowFlag(Qt::BypassGraphicsProxyWidget);

        scene = new QGraphicsScene();
        scene->setItemIndexMethod(QGraphicsScene::NoIndex);
        this->setScene(scene);

        this->setRenderHints(QPainter::Antialiasing);

        zoom = new GraphicsViewZoom(this);
        zoom->set_modifier(Qt::NoModifier);

        setTransformationAnchor(QGraphicsView::NoAnchor);
    }

    void setAction(Action action) {
        this->action = action;
    }

    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Shift) {
            shift_pressed = true;
        }
    }

    void keyReleaseEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Shift) {
            shift_pressed = false;
        }
    }

    void updateConnections() {
        for (int i = 0; i < connections.size(); i++) {
            connections[i].first->setLine(connectObjects(
                    dynamic_cast<PetriObject *>(connections[i].second.first),
                    dynamic_cast<PetriObject *>(connections[i].second.second)));
        }
    }

    void mousePressEvent(QMouseEvent *event) override {

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
            setDragMode(QGraphicsView::ScrollHandDrag);
            setInteractive(false);
        }

        if (action == Action::A_Move) QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {

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

    QGraphicsItem* itemAt(QPointF pos) {
        for (auto item: items) {
            if (item->sceneBoundingRect().contains(pos)) return item;
        }
        return nullptr;
    }

    void mouseReleaseEvent(QMouseEvent *event) override {

        setDragMode(QGraphicsView::NoDrag);
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

    QLineF connectObjects(PetriObject* from, PetriObject* to) {
        qreal from_angle = from->angleBetween(to->center());
        qreal to_angle = to->angleBetween(from->center());

        QPointF pointFrom = from->connectionPos(from_angle);
        QPointF pointTo = to->connectionPos(to_angle);

        return {pointFrom, pointTo};
    }


    void resizeEvent(QResizeEvent *event) override {
        scene->setSceneRect(this->pos().x(), this->pos().y(), 65535, 65535);
        QGraphicsView::resizeEvent(event);
    }

public slots:


private:

    typedef std::pair<QGraphicsLineItem*, std::pair<QGraphicsItem*, QGraphicsItem*>> Connection;

    QGraphicsScene* scene;
    QList<QGraphicsItem*> items;

    QList<Connection> connections;
    Connection* current_connection = nullptr;

    bool shift_pressed = false;

    Action action = Action::A_Nothing;

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;

};


#endif //FFI_RUST_GRAPHICS_VIEW_H
