//
// Created by darkp on 28.09.2022.
//

#include <QPainter>
#include <QToolTip>
#include <QStyleOptionGraphicsItem>
#include "reachability_node.h"

#define PEN_WIDTH 1

QString text_from_list(const QList<int32_t>& values) {
    QString result;
    for (auto value : values) {
        if (value < 0) result += QString("w|");
        else result += QString("%1|").arg(value);
    }

    if (result.endsWith("|")) result.chop(1);

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
    Q_UNUSED(widget)
    Q_UNUSED(option)
    if (!m_node) {
        return;
    }

    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(QPen(Qt::black, PEN_WIDTH));
    painter->drawPath(m_path);

    auto fields = (field_t*)ND_shape_info(m_node);
    auto label_x= m_boundingRect.topLeft().x();
    auto label_y = m_boundingRect.topLeft().y();
    for (int i = 0; i < fields->n_flds; i++) {
        // draw text
        auto field = fields->fld[i];
        auto text = field->lp->text;
        auto font = field->lp->fontname;
        auto font_size = field->lp->fontsize;

        auto rect = QRectF(label_x, label_y, field->size.x, field->size.y);

        painter->setFont(QFont(font, font_size));
        painter->drawText(rect, Qt::AlignCenter, text);

        label_x += field->size.x;
    }
}

void ReachabilityNode::updateLayout() {
    if (!m_node) return;

    auto info = (Agnodeinfo_t*)AGDATA(m_node);
    auto [x, y] = info->coord;

    setPos(x, y);

    qreal w = info->width * 72.;
    qreal h = info->height * 72.;

    m_boundingRect =  { - w / 2.,
                        - h / 2.,
                        w,
                        h };

    m_path = QPainterPath();
    m_path.addRect(m_boundingRect);

    auto label_x= m_boundingRect.topLeft().x();
    auto label_y = m_boundingRect.topLeft().y() + PEN_WIDTH;
    field_t* shape = (field_t*)ND_shape_info(m_node);
    for (int i = 0; i < shape->n_flds; i++) {
        auto field = shape->fld[i];

        if (i < shape->n_flds - 1) {
            label_x += field->size.x;
            m_path.moveTo(label_x, label_y);
            m_path.lineTo(label_x, label_y + field->size.y - PEN_WIDTH);
        }
    }

    update();
}
