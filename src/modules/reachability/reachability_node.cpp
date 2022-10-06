//
// Created by darkp on 28.09.2022.
//

#include <QPainter>
#include <QToolTip>
#include <QStyleOptionGraphicsItem>
#include "reachability_node.h"

QString text_from_list(const QList<int32_t>& values) {
    QString result;
    for (auto value : values) {
        if (value < 0) result += QString("w|");
        else result += QString("%1|").arg(value);
    }
    return result;
}

ReachabilityNode::ReachabilityNode(QList<int32_t> values, QGraphicsItem *parent): QGraphicsItem(parent) {
    m_node = nullptr;
    m_values = std::move(values);
    m_text = text_from_list(m_values);

    setToolTip(m_text);
    setAcceptHoverEvents(true);
}

const QString &ReachabilityNode::text() const {
    return m_text;
}

void ReachabilityNode::setGraphVizNode(Agnode_s *node) {
    m_node = node;
}

Agnode_s* ReachabilityNode::graphVizNode() {
    return m_node;
}

QRectF ReachabilityNode::boundingRect() const {
    return m_boundingRect;
}


void ReachabilityNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!m_node) {
        //QGraphicsItem::paint(painter, option, widget);
        return;
    }

    auto info = (Agnodeinfo_t*)AGDATA(m_node);
    auto [x, y] = info->coord;

    auto rect = boundingRect();
    auto center = rect.center();
    painter->drawRect(rect);

    auto label = info->label;
    QFontMetricsF metrics(QFont(label->fontname, (int)10));
    QSizeF text_size = metrics.size(0, label->text);
    qreal x_label = center.x() - text_size.width() / 2.;
    qreal y_label = center.y() + text_size.height() / 2.;

    painter->save();
    painter->setFont(QFont(info->label->fontname, 10));
    painter->drawText((int)x_label, (int)y_label, label->text);
    painter->restore();

}

void ReachabilityNode::updateLayout() {
    if (!m_node) return;

    auto info = (Agnodeinfo_t*)AGDATA(m_node);
    auto [x, y] = info->coord;

    setPos(x, y);

    qreal w = info->width * 72.;
    qreal h = info->height * 72.;

    m_boundingRect =  { - w / 2., - h / 2., w, h };

    update();
}
