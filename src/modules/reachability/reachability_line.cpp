//
// Created by darkp on 05.10.2022.
//

#include <QPainter>
#include "reachability_line.h"

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
    auto info = (Agedgeinfo_t*)AGDATA(m_edge);

    auto points_count = info->spl->list->size;
    auto points = info->spl->list->list;

    QPainterPath line;
    line.moveTo(points[0].x, points[0].y);
    for (int i = 1; i < points_count; i++) {
        line.lineTo(points[i].x, points[i].y);
    }
    line.lineTo(info->spl->list->ep.x, info->spl->list->ep.y);

    painter->drawPath(line);
    //QGraphicsLineItem::paint(painter, option, widget);
}
