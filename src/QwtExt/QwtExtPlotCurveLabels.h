//
// Created by darkp on 12.09.2022.
//

#ifndef FFI_RUST_QWT_EXT_PLOT_CURVE_LABELS_H
#define FFI_RUST_QWT_EXT_PLOT_CURVE_LABELS_H

#include <qwt_plot_curve.h>
#include <QHash>

class QwtExtPlotCurveLabels : public QwtPlotCurve {

public:

    explicit QwtExtPlotCurveLabels(const QString &title = QString());

    void drawDots(QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect, int from, int to) const override;

    void setData(QHash<QPoint, QVector<std::size_t>>&& data);
    const QVector<std::size_t>& getData(int idx) const;

private:

    QHash<QPoint, QVector<std::size_t>> m_data;

};


#endif //FFI_RUST_QWT_EXT_PLOT_CURVE_LABELS_H
