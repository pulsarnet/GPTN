//
// Created by nmuravev on 12/13/2021.
//

#include "position.h"
#include "transition.h"

Position::Position(const QPointF& origin, ffi::Position* position, QGraphicsItem *parent) : PetriObject(parent) {
    this->setPos(origin);
    this->m_position = position;
}

QRectF Position::boundingRect() const {
    qreal diameter = 2. * radius;
    return {-radius, -radius,
            diameter, diameter};
}

void Position::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    qreal penWidth = 2;

    painter->save();
    painter->setPen(QPen(isSelected() ? Qt::red : painter->pen().color(), penWidth));
    painter->drawEllipse(boundingRect().center(), radius - penWidth / 2., radius - penWidth / 2.);

    auto name = QString("p%1%2").arg(this->index()).arg(this->markers() == 0 ? "" : QString("(%1)").arg(
                                                                this->markers()));

    painter->drawText(boundingRect(), Qt::AlignCenter, name);
    painter->restore();
}

QPointF Position::center() {
    return scenePos();
}

QPointF Position::connectionPos(PetriObject* to, bool reverse) {

    qreal angle = this->angleBetween(to->scenePos());

    qreal x = scenePos().x();
    qreal y = scenePos().y();

    angle = angle - qDegreesToRadians(90);
    qreal xPosy = (x + qCos(angle) * radius);
    qreal yPosy = (y + qSin(angle) * radius);

    return {xPosy, yPosy};
}

void Position::connectTo(ffi::PetriNet *net, PetriObject *other) {
    auto transition = dynamic_cast<class Transition*>(other)->transition();
    net->connect_p(this->position(), transition);
}

uint64_t Position::index() const {
    return m_position->index();
}

void Position::add_marker() {
    m_position->add_marker();
}

void Position::remove_marker() {
    m_position->remove_marker();
}

unsigned long Position::markers() {
    return m_position->markers();
}

void Position::setMarkers(unsigned long count) {
    //TODO: m_position->set_markers(count);
}

QString Position::name() {
    return QString("p%1").arg(m_position->index());
}

