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
#include "position.h"
#include "arrow_line.h"

class GraphicsView : public QGraphicsView {

public:
    enum Action {
        A_Position,
        A_Transition,
        A_Connect,
        A_Move,
        A_Nothing
    };

public:

    GraphicsView(QWidget* parent = nullptr): QGraphicsView(parent) {
        scene = new QGraphicsScene();

        this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        this->setMinimumHeight(100);
        this->setMinimumWidth(100);

        this->setScene(scene);
        this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
        scene->setItemIndexMethod(QGraphicsScene::NoIndex);
        this->setWindowFlag(Qt::BypassGraphicsProxyWidget);

        this->setRenderHints(QPainter::Antialiasing);
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

    void mousePressEvent(QMouseEvent *event) override {

        if (event->button() == Qt::LeftButton) {
            if (action == Action::A_Position) {
                items.push_back(new Position);
                items.back()->setPos(event->scenePosition());
                scene->addItem(items.back());
            }
            else if (action == Action::A_Transition) {
                items.push_back(new Transition);
                items.back()->setPos(event->scenePosition());
                scene->addItem(items.back());
            }
            else if (auto item = scene->itemAt(event->scenePosition(), transform()); item && action == Action::A_Connect) {
                auto line_item = new ArrowLine(QLineF(item->scenePos().x(), item->scenePos().y(), event->scenePosition().x(), event->scenePosition().y()));
                scene->addItem(line_item);
                connections.push_back(std::make_pair(line_item, std::make_pair(item, nullptr)));
                current_connection = &connections.back();
            }
        }

        if (action == Action::A_Move) QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {

        if (event->buttons() & Qt::LeftButton && action == Action::A_Connect && current_connection) {
            auto line = current_connection->first->line();
            line.setP2(event->scenePosition());
            current_connection->first->setLine(line);
        }
        else  {
            for (int i = 0; i < connections.size(); i++) {
                connections[i].first->setLine(connectObjects(
                        dynamic_cast<PetriObject *>(connections[i].second.first),
                        dynamic_cast<PetriObject *>(connections[i].second.second)));
            }
        }

        if (action == Action::A_Move) QGraphicsView::mouseMoveEvent(event);
    }

    QGraphicsItem* itemAt(QPointF pos) {
        for (auto item: items) {
            if (item->sceneBoundingRect().contains(pos)) return item;
        }
        return nullptr;
    }

    QLineF connectObjects(PetriObject* from, PetriObject* to) {
        qreal from_angle = from->angleBetween(to->center());
        qreal to_angle = to->angleBetween(from->center());

        QPointF pointFrom = from->connectionPos(from_angle);
        QPointF pointTo = to->connectionPos(to_angle);

        return {pointFrom, pointTo};
    }

    void mouseReleaseEvent(QMouseEvent *event) override {

        if (auto item = itemAt(event->scenePosition()); item && current_connection) {
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

    void resizeEvent(QResizeEvent *event) override {
        scene->setSceneRect(this->pos().x(), this->pos().y(), this->width(), this->height());
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


};


#endif //FFI_RUST_GRAPHICS_VIEW_H
