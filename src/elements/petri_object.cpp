//
// Created by Николай Муравьев on 10.01.2022.
//

#include "position.h"


PetriObject::PetriObject(QGraphicsItem *parent) : QGraphicsItem(parent) {
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable);
}

void PetriObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }

    QGraphicsItem::mousePressEvent(event);
}

void PetriObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
}

void PetriObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

QPointF PetriObject::connectionPos(PetriObject* to, bool reverse) {
    return {0, 0};
}

qreal PetriObject::angleBetween(const QPointF &to) {
    qreal x = center().x();
    qreal y = center().y();

    qreal deltaX = to.x() - x;
    qreal deltaY = to.y() - y;

    qreal rotation = -atan2(deltaX, deltaY);
    rotation = qDegreesToRadians(qRadiansToDegrees(rotation) + 180.0);

    return rotation;
}