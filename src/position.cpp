//
// Created by nmuravev on 12/13/2021.
//

#include <format>
#include "../include/position.h"

PetriObject::PetriObject(QGraphicsItem *parent) : QGraphicsItem(parent) {
    setFlags(ItemIsMovable | ItemSendsGeometryChanges);
}

void PetriObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }

    Q_UNUSED(event)
}

void PetriObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        this->setPos(mapToScene(event->pos()));
    }

    Q_UNUSED(event)
}

void PetriObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    Q_UNUSED(event)
}

QPointF PetriObject::connectionPos(qreal angle) {
    return {0, 0};
}

qreal PetriObject::angleBetween(const QPointF &to) {
    qreal x = center().x();
    qreal y = center().y();

    qreal deltaX = to.x() - x;
    qreal deltaY = to.y() - y;

    qreal rotation = -atan2(deltaX, deltaY);
    rotation = qDegreesToRadians(qRadiansToDegrees(rotation) + 180);

    return rotation;
}

Position::Position(const QPointF& origin, uint32_t index, QGraphicsItem *parent) : PetriObject(parent) {
    this->setPos(origin);
    this->m_index = index;
}

QRectF Position::boundingRect() const {
    qreal penWidth = 1;
    qreal diameter = 2 * radius;
    return {-radius - penWidth / 2, -radius - penWidth / 2,
            diameter + penWidth, diameter + penWidth};
}

void Position::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    painter->drawEllipse(boundingRect().center(), radius, radius);

    auto name = QString::fromStdString(std::format("p{}", this->index()));
    painter->drawText(boundingRect().center() - QPointF(5.0, 5.0), name);
    painter->restore();
    //painter->drawText(boundingRect().center(), QTextFormat())
}

QPointF Position::center() {
    return scenePos();
}

QPointF Position::connectionPos(qreal angle) {
    double x = scenePos().x();
    double y = scenePos().y();

    angle = angle - qDegreesToRadians(90);
    qreal xPosy = round((x + qCos(angle) * radius));
    qreal yPosy = round((y + qSin(angle) * radius));

    return {xPosy, yPosy};
}

Transition::Transition(const QPointF& origin, uint32_t index, QGraphicsItem *parent) : PetriObject(parent), m_origin(origin) {
    this->setPos(m_origin);
    this->m_index = index;
}

QRectF Transition::boundingRect() const {
    return {-10, -30, 20, 60};
}

void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();
    painter->setBrush(Qt::black);
    painter->drawRect(boundingRect());

    painter->setPen(Qt::white);
    auto name = QString::fromStdString(std::format("t{}", this->index()));
    painter->drawText(boundingRect().left(), boundingRect().center().y(), name);
    painter->restore();
}

QPointF Transition::center() {
    return this->scenePos();
}

QPointF Transition::connectionPos(qreal angle) {
    double x = center().x();
    double y = center().y();

    angle = angle - qDegreesToRadians(90);
    qreal xPosy = round((x + qCos(angle) * sceneBoundingRect().width() / 2.));
    qreal yPosy = round((y + qSin(angle) * sceneBoundingRect().height() / 2.));

    return {xPosy, yPosy};
}
