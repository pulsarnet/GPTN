//
// Created by darkp on 12.09.2022.
//

#include "qwt_ext_plot_curve_labels.h"
#include <qwt_painter.h>
#include <qwt_scale_map.h>

QwtExtPlotCurveLabels::QwtExtPlotCurveLabels(const QString &title) : QwtPlotCurve(title) {

}

void
QwtExtPlotCurveLabels::drawDots(QPainter * painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect,
                                int from, int to) const {

    auto series = data();
    for (int i = from; i <= to; i++) {
        const QPointF sample = series->sample(i);

        long x = (long)sample.x();
        long y = (long)sample.y();

        auto it = this->m_data.find(QPoint(x, y));
        assert(it != this->m_data.end());

        double xi = xMap.transform(sample.x());
        double yi = yMap.transform(sample.y());

        QwtPainter::drawText(painter, xi, yi, QString("%3(%1,%2)").arg(x).arg(y).arg(it.value()));
    }
    QwtPlotCurve::drawDots(painter, xMap, yMap, canvasRect, from, to);
}

void QwtExtPlotCurveLabels::setData(QHash<QPoint, std::size_t>&& data) {
    this->m_data = std::move(data);

    QVector<QPointF> samples;
    auto keys = this->m_data.keys();
    for (const auto key : keys) {
        samples.push_back(QPointF(key));
    }

    this->setSamples(samples);
}
