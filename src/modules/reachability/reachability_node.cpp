//
// Created by darkp on 28.09.2022.
//

#include <QPainter>
#include "reachability_node.h"

QString text_from_list(const QList<int32_t>& values) {
    QString result;
    for (auto value : values) {
        if (value < 0) result += QString("w|");
        else result += QString("%1|").arg(value);
    }
    return result;
}

ReachabilityNode::ReachabilityNode(QList<int32_t> values, QObject *parent) {
    m_node = nullptr;
    m_values = std::move(values);
    m_text = text_from_list(m_values);
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
    if (!m_node) return QGraphicsRectItem::boundingRect();

    auto info = (Agnodeinfo_t*)AGDATA(m_node);
    return QRectF(
            QPointF(info->bb.LL.x, info->bb.LL.y),
            QPointF(info->bb.UR.x, info->bb.UR.y)
            );
}


void ReachabilityNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (!m_node) {
        QGraphicsRectItem::paint(painter, option, widget);
        return;
    }

    auto info = (Agnodeinfo_t*)AGDATA(m_node);
    auto [x, y] = info->coord;
    qreal w = info->width * 72;
    qreal h = info->height * 72;

    painter->drawRect(
            QRectF(x - w / 2, y - h / 2, w, h)
            );

    auto label = info->label;
    QFontMetricsF metrics(QFont(label->fontname, (int)label->fontsize));
    QSizeF text_size = metrics.size(0, label->text);
    qreal x_label = x - text_size.width() / 2;
    qreal y_label = y;

    painter->drawText((int)x_label, (int)y_label, label->text);

}