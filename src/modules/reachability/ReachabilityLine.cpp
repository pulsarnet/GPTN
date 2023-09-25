//
// Created by darkp on 05.10.2022.
//

#include <QPainter>
#include "ReachabilityLine.h"

ReachabilityLine::ReachabilityLine(Agedge_s *edge, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_edge(edge)
{
    Q_ASSERT(edge != nullptr);
}

QRectF ReachabilityLine::boundingRect() const {
    auto info = (Agedgeinfo_t*)AGDATA(m_edge);

    auto points_count = info->spl->list->size;
    auto points = info->spl->list->list;

    QPolygonF polygon;
    for (int i = 0; i < points_count; i++) {
        polygon << QPointF(points[i].x, points[i].y);
    }

    polygon << QPointF(info->spl->list->ep.x, info->spl->list->ep.y);

    return polygon.boundingRect();
}

void ReachabilityLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    auto info = (Agedgeinfo_t*)AGDATA(m_edge);

    auto points_count = info->spl->list->size;
    auto points = info->spl->list->list;

    QPainterPath line;
    line.moveTo(points[0].x, points[0].y);
    for (int i = 1; i < points_count; i++) {
        line.lineTo(points[i].x, points[i].y);
    }
    painter->drawPath(line);

    static const size_t arrowSize = 10;

    QLineF directLine(
            QPointF(info->spl->list->list[points_count - 1].x, info->spl->list->list[points_count - 1].y),
            QPointF(info->spl->list->ep.x, info->spl->list->ep.y)
            );

    QLineF normal = directLine.normalVector();
    QPointF o(normal.dx() / 3.0, normal.dy() / 3.0);

    QPolygonF arrow;
    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::black));
    arrow << directLine.p1() + o << directLine.p2() << directLine.p1() - o;
    painter->drawPolygon(arrow);
    painter->restore();

    // label
    auto label = QString::fromUtf8(info->label->text);
    auto pos = info->label->pos;
    painter->drawText((int)pos.x, (int)pos.y, label);

    //QGraphicsLineItem::paint(painter, option, widget);
}
