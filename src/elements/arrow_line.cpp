//
// Created by nmuravev on 12/14/2021.
//

#include "arrow_line.h"

QRectF ArrowLine::boundingRect() const {
    auto rect = QGraphicsLineItem::boundingRect();
    rect.setX(rect.x() - 6.);
    rect.setY(rect.y() - 6.);
    rect.setWidth(rect.width() + 12);
    rect.setHeight(rect.height() + 12);
    return rect;
}

void ArrowLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    qreal arrowSize = 10;

    QLineF line(this->line().p2(), this->line().p1());

    if (line.isNull()) return;

    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI / 3.) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI - M_PI / 3.) * arrowSize);

    QPointF arrowLineCenter = QLineF(arrowP1, arrowP2).center();
    QPointF endPoint = line.p1();
    endPoint.setX(arrowLineCenter.x());
    endPoint.setY(arrowLineCenter.y());

    QPolygonF arrowHead;
    arrowHead.clear();
    arrowHead << line.p1() << arrowP1 << arrowP2;

    painter->save();
    painter->setBrush(isSelected() ? Qt::red : Qt::black);
    painter->setPen(QPen(isSelected() ? Qt::red : painter->pen().color(), LINE_WIDTH));
    painter->drawLine(QLineF(endPoint, line.p2()));

    painter->setPen(Qt::NoPen);
    painter->drawPolygon(arrowHead);
    painter->restore();
}


ArrowLine::ArrowLine(PetriObject *from, const QLineF &line, QGraphicsItem *parent) : QGraphicsLineItem(line, parent) {
    this->m_from = from;

    setFlags(ItemIsSelectable);
}
