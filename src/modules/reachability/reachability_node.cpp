//
// Created by darkp on 28.09.2022.
//

#include <QPainter>
#include <QToolTip>
#include <QStyleOptionGraphicsItem>
#include "reachability_node.h"

#define PEN_WIDTH 1

QString create_text(const QList<int32_t>& values) {
    QString result;
    for (auto value : values) {
        if (value < 0) result += QString("w|");
        else result += QString("%1|").arg(value);
    }

    if (result.endsWith("|")) result.chop(1);

    return result;
}

QString create_text_with_headers(const QList<int32_t>& values, const QList<ffi::VertexIndex>& headers) {
    Q_ASSERT(values.size() == headers.size());
    QString result;
    auto it_h = QListIterator(headers);
    auto it_v = QListIterator(values);
    while (it_h.hasNext() && it_v.hasNext()) {
        result += QString("{P%1|%2}|")
                .arg(it_h.peekNext().id)
                .arg(it_v.peekNext() < 0 ? QString("w") : QString::number(it_v.peekNext()));
        it_h.next();
        it_v.next();
    }
    if (result.endsWith("|")) result.chop(1);
    qDebug() << result;
    return result;
}

ReachabilityNode::ReachabilityNode(QList<int32_t> values, QGraphicsItem *parent): QGraphicsItem(parent) {
    m_values = std::move(values);
    m_text = create_text(m_values);

    setToolTip(m_text);
    setAcceptHoverEvents(true);
}

ReachabilityNode::ReachabilityNode(QList<int32_t> values, QList<ffi::VertexIndex> headers, QGraphicsItem *parent): QGraphicsItem(parent) {
    m_values = std::move(values);
    m_text = create_text_with_headers(m_values, headers);

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

void ReachabilityNode::setType(rust::MarkingType type) {
    m_type = type;
}

rust::MarkingType ReachabilityNode::getType() const {
    return m_type;
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

    auto type = getType();
    QColor penColor;
    switch (type) {
        case rust::MarkingType::Duplicate:
            penColor = Qt::blue;
            break;
        case rust::MarkingType::DeadEnd:
            penColor = Qt::red;
            break;
        case rust::MarkingType::Inner:
        case rust::MarkingType::Boundary:
        default:
            penColor = Qt::black;
    }

    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(QPen(penColor, PEN_WIDTH));
    painter->drawPath(m_path);

    auto fields = (field_t*)ND_shape_info(m_node);
    for (int i = 0; i < fields->n_flds; i++) {
        paintField(painter, fields->fld[i]);
    }
}

void ReachabilityNode::paintField(QPainter *painter, field_t* fld) {
    if (!fld) return;
    if (fld->n_flds == 0) {
//        auto label_x= m_boundingRect.topLeft().x();
//        auto label_y = m_boundingRect.topLeft().y();
        // draw text
        auto text = fld->lp->text;
        auto font = fld->lp->fontname;
        auto font_size = fld->lp->fontsize;

        auto rect = QRectF(fld->b.LL.x, -fld->b.UR.y, fld->size.x, fld->size.y);

        painter->setFont(QFont(font, font_size));
        painter->drawText(rect, Qt::AlignCenter, text);
    } else {
        for (int i = 0; i < fld->n_flds; i++) {
            paintField(painter, fld->fld[i]);
        }
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
    // todo horizontal line
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
