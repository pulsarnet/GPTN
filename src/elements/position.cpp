//
// Created by nmuravev on 12/13/2021.
//

#include <fmt/format.h>
#include "../../include/elements/position.h"
#include "../../include/elements/transition.h"

Position::Position(const QPointF& origin, FFIPosition* position, QGraphicsItem *parent) : PetriObject(parent) {
    this->setPos(origin);
    this->m_position = position;
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

    auto name = QString::fromStdString(
            fmt::format("p{}{}",
                        this->index(),
                        this->markers() == 0 ? "" : fmt::format("({})",
                                                                this->markers()))
            );

    painter->drawText(boundingRect(), Qt::AlignCenter, name);
    painter->restore();
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

void Position::connectTo(PetriNet *net, PetriObject *other) {
    auto transition = dynamic_cast<class Transition*>(other)->transition();
    net->connect_p(this->position(), transition);
}

