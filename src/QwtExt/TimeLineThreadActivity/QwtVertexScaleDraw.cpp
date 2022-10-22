//
// Created by darkp on 22.10.2022.
//

#include "QwtVertexScaleDraw.h"
#include <QwtText>
#include <QPainter>
#include <QWidget>

QwtVertexScaleDraw::QwtVertexScaleDraw() : QwtScaleDraw() {
    enableComponent(QwtScaleDraw::Backbone, false);
    enableComponent(QwtScaleDraw::Ticks, false);
    enableComponent(QwtScaleDraw::Labels, true);
}

void QwtVertexScaleDraw::setData(const QHash<int, QString> &data) {
    m_data = data;
}

QwtText QwtVertexScaleDraw::label(double value) const {
    auto point = (int)ceil(value);
    if (ceil(value) == value) {
        if (m_data.contains(point)) {
            return m_data[point];
        }
    }
    return QwtText();
}

void QwtVertexScaleDraw::drawLabel(QPainter *painter, double value) const {
    QwtText label = this->label(value);
    if (label.isEmpty())
        return;

    int upper = (int)floor(value) + 1;
    int lower = (int)floor(value);

    label.setRenderFlags(Qt::AlignCenter);

    QPointF upperPoint = labelPosition(upper);
    QPointF lowerPoint = labelPosition(lower);
    QPointF center = (upperPoint + lowerPoint) / 2;
    QSizeF size = label.textSize(painter->font());

    const QTransform transform = labelTransformation(center, size);
    painter->save();
    painter->setWorldTransform(transform, true);
    label.draw(painter, QRectF(QPointF(0, 0), size.toSize()));
    painter->restore();
}
