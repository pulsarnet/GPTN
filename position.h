//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_POSITION_H
#define FFI_RUST_POSITION_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>


class PetriObject : public QObject, public QGraphicsItem {

public:

    PetriObject(QGraphicsItem* parent = nullptr) : QGraphicsItem(parent) {
        setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    }

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            this->setCursor(QCursor(Qt::ClosedHandCursor));
        }

        Q_UNUSED(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            this->setPos(mapToScene(event->pos()));
        }

        Q_UNUSED(event);
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
        Q_UNUSED(event);
    }

};

class Position : public PetriObject {

public:

    explicit Position(QGraphicsItem* parent = nullptr): PetriObject(parent) {

    }

    QRectF boundingRect() const override {
        qreal penWidth = 1;
        qreal diameter = 2 * radius;
        return QRectF(-radius - penWidth / 2, -radius - penWidth / 2,
                      diameter + penWidth, diameter + penWidth);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawEllipse(boundingRect().center(), boundingRect().height() / 4 - 7, boundingRect().height() / 4 - 7);
    }

private:

    qreal radius = 50.0;

};

class Transition : public PetriObject {

public:

    explicit Transition(QGraphicsItem* parent = nullptr): PetriObject(parent) {}

    QRectF boundingRect() const override {
        return QRectF(0, 0, 20, 60);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        painter->drawRect(boundingRect());
    }

};


#endif //FFI_RUST_POSITION_H
