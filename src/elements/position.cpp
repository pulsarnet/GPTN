#include "position.h"
#include "transition.h"

Position::Position(const QPointF& origin, ffi::Vertex* position, QGraphicsItem *parent) : PetriObject(position, parent) {
    this->setPos(origin);
}

QRectF Position::boundingRect() const {
    return shape().boundingRect().adjusted(-2, -2, 2, 2);
}

void Position::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    qreal penWidth = 2;

    painter->setClipRect(boundingRect());

    painter->save();
    painter->setPen(QPen(isSelected() ? Qt::red : painter->pen().color(), penWidth));
    painter->drawEllipse(boundingRect().center(), radius - penWidth / 2., radius - penWidth / 2.);

    auto name = QString("%1%2").arg(this->index()).arg(this->markers() == 0 ? "" : QString("(%1)").arg(
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

void Position::add_marker() {
    vertex()->add_marker();
}

void Position::remove_marker() {
    vertex()->remove_marker();
}

unsigned long Position::markers() {
    return vertex()->markers();
}

void Position::setMarkers(unsigned long count) {
    //TODO: m_position->set_markers(count);
}

QString Position::name() const {
    return QString("p%1").arg(index());
}

QPainterPath Position::shape() const {
    QPainterPath path;
    path.addEllipse({0, 0}, radius, radius);
    return path;
}

