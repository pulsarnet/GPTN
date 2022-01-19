//
// Created by nmuravev on 12/14/2021.
//

#include "arrow_line.h"

QRectF ArrowLine::boundingRect() const {

    QRectF rect;

    if (m_to) {
        rect = m_shape.boundingRect().adjusted(-10., -10, 10, 10);
    }
    else {
        rect = QGraphicsLineItem::boundingRect();
        rect.setX(rect.x() - 6.);
        rect.setY(rect.y() - 6.);
        rect.setWidth(rect.width() + 12);
        rect.setHeight(rect.height() + 12);
    }
    return rect;
}

void ArrowLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    painter->setClipRect(boundingRect());

    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    painter->drawPath(m_shape);

    painter->setBrush(Qt::black);
    painter->setPen(Qt::NoPen);
    painter->drawPolygon(m_arrow);
}


ArrowLine::ArrowLine(PetriObject *from, const QLineF &line, QGraphicsItem *parent) : QGraphicsLineItem(line, parent) {
    this->m_from = from;

    setFlags(ItemIsSelectable);
}

bool ArrowLine::setTo(PetriObject *to) {
    if (this->m_from->allowConnection(to)) {
        if (m_to) m_to->removeConnectionLine(this);
        m_to = to;

        m_from->addConnectionLine(this);
        m_to->addConnectionLine(this);
        return true;
    }

    return false;
}

void ArrowLine::disconnect(PetriNet *net) {
    if (auto pos = dynamic_cast<Position*>(this->from()); pos) {
        net->remove_connection_p(pos->position(), dynamic_cast<Transition*>(this->to())->transition());
    }
    else if (auto transition = dynamic_cast<Transition*>(this->to()); transition) {
        net->remove_connection_t(transition->transition(), dynamic_cast<Position*>(this->to())->position());
    }

    this->from()->removeConnectionLine(this);
    this->to()->removeConnectionLine(this);
}

void ArrowLine::updateConnection() {

    qreal arrowSize = 10;

    bool firstPosition = dynamic_cast<Position*>(from());

    if (from() && to()) {
        auto point1 = from()->connectionPos(to(), true);
        auto point2 = to()->connectionPos(from(), false);

        setLine(QLineF(point1, point2));
    }

    QLineF line(this->line().p2(), this->line().p1());

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI / 3.) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI - M_PI / 3.) * arrowSize);

    QPointF arrowLineCenter = QLineF(arrowP1, arrowP2).center();
    QPointF endPoint = line.p1();
    endPoint.setX(arrowLineCenter.x());
    endPoint.setY(arrowLineCenter.y());

    m_arrow.clear();
    m_arrow << line.p1() << arrowP1 << arrowP2;

    QPainterPath curve;
    if (m_bendFactor) {
        qreal posFactor = qAbs(m_bendFactor);
        bool bendDirection = firstPosition;
        if (m_bendFactor < 0)
            bendDirection = !bendDirection;

        QLineF f1((endPoint + line.p2()) / 2., line.p2());
        f1.setAngle(bendDirection ? f1.angle() + 90 : f1.angle() - 90);
        f1.setLength(f1.length() * 0.2 * posFactor);

        auto m_controlPos = f1.p2();
        auto m_controlPoint = m_controlPos - ((endPoint + line.p2()) / 2. - m_controlPos) * 0.13;

        curve.moveTo(endPoint);
        curve.cubicTo(m_controlPoint, m_controlPoint, line.p2());
    }
    else {
        // direction line
        curve.moveTo(endPoint);
        curve.lineTo(line.p2());
    }

    m_shape = curve;
}

void ArrowLine::setBendFactor(int bf) {
    m_bendFactor = bf;

    updateConnection();
}
