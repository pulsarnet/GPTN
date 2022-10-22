//
// Created by darkp on 22.10.2022.
//

#include "QwtCyclesScaleDraw.h"
#include <QwtScaleMap>
#include <QDebug>
#include <QwtText>
#include <QPainter>

QwtCyclesScaleDraw::QwtCyclesScaleDraw() : QwtScaleDraw() {
    enableComponent(QwtAbstractScaleDraw::Ticks, true);
    enableComponent(QwtAbstractScaleDraw::Backbone, true);
    enableComponent(QwtAbstractScaleDraw::Labels, true);

    setScaleDiv(QwtScaleDiv(0, 1));
}

void QwtCyclesScaleDraw::drawTick(QPainter *painter, double value, double len) const {
    if (ceil(value) != value) {
        return;
    }

    // draw tick
    QwtScaleDraw::drawTick(painter, value, len);
}

void QwtCyclesScaleDraw::drawLabel(QPainter *painter, double value) const {
    if (ceil(value) != value) {
        return;
    }

    // draw label
    QwtScaleDraw::drawLabel(painter, value);
}
