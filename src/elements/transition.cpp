//
// Created by Николай Муравьев on 10.01.2022.
//

#include <fmt/format.h>
#include "../../include/elements/position.h"
#include "transition.h"
#include "../../include/elements/transition.h"

Transition::Transition(const QPointF& origin, FFITransition* transition, QGraphicsItem *parent) : PetriObject(parent), m_origin(origin) {
    this->setPos(m_origin);
    this->m_transition = transition;
}

QRectF Transition::boundingRect() const {
    return {-10, -30, 20, 60};
}

void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->save();

    if (PetriObject::colored()) painter->setPen(Qt::red);
    painter->setBrush(Qt::black);
    painter->drawRect(boundingRect());

    painter->setPen(Qt::white);
    auto name = QString::fromStdString(fmt::v8::format("t{}", this->index()));
    painter->drawText(boundingRect().left(), boundingRect().center().y(), name);
    painter->restore();
}

QPointF Transition::center() {
    return this->scenePos();
}

QPointF Transition::connectionPos(PetriObject* to, bool reverse) {


    qreal angle = this->angleBetween(to->pos());

    double x = QGraphicsItem::scenePos().x();
    double y = QGraphicsItem::scenePos().y();

    qreal w = QGraphicsItem::sceneBoundingRect().width() / 2.;
    qreal h = QGraphicsItem::sceneBoundingRect().height() / 2.;


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