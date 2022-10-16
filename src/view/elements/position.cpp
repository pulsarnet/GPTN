#include "position.h"
#include "transition.h"
#include <QFontMetrics>

Position::Position(const QPointF& origin, ffi::PetriNet* net, ffi::VertexIndex position, QGraphicsItem* parent) : PetriObject(net, position, parent) {
    this->setPos(origin);
}

QRectF Position::boundingRect() const {
    return shape().boundingRect().adjusted(-2, -20, 2, 20);
}

void Position::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    qreal penWidth = 2;

    painter->save();
    painter->setPen(QPen(isSelected() ? Qt::green : painter->pen().color(), penWidth));
    painter->drawEllipse(boundingRect().center(), radius - penWidth / 2., radius - penWidth / 2.);

    auto name = QString("P%1%2")
            .arg(this->index())
            .arg(this->vertex()->parent() == 0 ? "" : QString(".%1").arg(this->vertex()->parent()));

    QFontMetricsF metric = QFontMetricsF(painter->font());
    QSizeF nameSize = metric.size(0, name);
    painter->drawText(boundingRect().center().x() - nameSize.width() / 2
                      , boundingRect().center().y() - radius - 5
                      , name);

    if (this->markers() == 1) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));
        painter->drawEllipse(boundingRect().center(), 3., 3.);
        painter->restore();
    } else if (this->markers() == 2) {
        auto rect = boundingRect();
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (this->markers() == 3) {
        auto rect = boundingRect();
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() + 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() - 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (this->markers() == 4) {
        auto rect = boundingRect();
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() + 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x());
        rect.setY(rect.y() - 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 18.);
        rect.setY(rect.y());
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (this->markers() == 5) {
        auto rect = boundingRect();
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 12.);
        rect.setY(rect.y() + 12.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 24.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x());
        rect.setY(rect.y() - 24.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 24.);
        rect.setY(rect.y());
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 12.);
        rect.setY(rect.y() + 12.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (this->markers() > 5) {
        painter->drawText(boundingRect(), Qt::AlignCenter, QString("%1").arg(this->markers()));
    }

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

