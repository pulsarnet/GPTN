//
// Created by nmuravev on 12/13/2021.
//

#include <fmt/format.h>
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

QPointF PetriObject::connectionPos(PetriObject* to, bool reverse) {
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
    if (colored()) painter->setPen(Qt::red);

    painter->drawEllipse(boundingRect().center(), radius, radius);

    auto name = QString::fromStdString(fmt::format("p{}", this->index()));
    painter->drawText(boundingRect().center() - QPointF(5.0, 5.0), name);
    painter->restore();
    //painter->drawText(boundingRect().center(), QTextFormat())
}

QPointF Position::center() {
    return scenePos();
}

QPointF Position::connectionPos(PetriObject* to, bool reverse) {

    qreal angle = this->angleBetween(to->pos());

    double x = scenePos().x();
    double y = scenePos().y();

    angle = angle - qDegreesToRadians(reverse ? 100 : 80);
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

    if (colored()) painter->setPen(Qt::red);
    painter->setBrush(Qt::black);
    painter->drawRect(boundingRect());

    painter->setPen(Qt::white);
    auto name = QString::fromStdString(fmt::format("t{}", this->index()));
    painter->drawText(boundingRect().left(), boundingRect().center().y(), name);
    painter->restore();
}

QPointF Transition::center() {
    return this->scenePos();
}

QPointF getIntersection(qreal dx, qreal dy, qreal cx, qreal cy, qreal width, qreal height) {
    if (qAbs(dy / dx) < height / width) {
        return QPointF(cx + (dx > 0 ? width : -width), cy + dy * width / qAbs(dx));
    }
    return QPointF(cx + dx * height / qAbs(dy), cy + (dy > 0 ? height : -height));
}

QPointF Transition::connectionPos(PetriObject* to, bool reverse) {


    qreal angle = this->angleBetween(to->pos());

    double x = scenePos().x();
    double y = scenePos().y();

    qreal w = sceneBoundingRect().width() / 2.;
    qreal h = sceneBoundingRect().height() / 2.;


    angle = angle - qDegreesToRadians(reverse ? 100 : 80);
    qreal xPosy = round((x + qCos(angle) * w));
    qreal yPosy = round((y + qSin(angle) * h));

    qreal dx = xPosy - center().x();
    qreal dy = yPosy - center().y();

    qreal cx = center().x();
    qreal cy = center().y();


    auto intersection = getIntersection(dx, dy,
                                        cx, cy,
                                        w, h);


    return intersection;
}
