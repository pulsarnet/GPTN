//
// Created by darkp on 12.09.2022.
//

#pragma once
#include <QObject>

class QPointF;
class QwtPlot;
class QwtPlotCurve;

class CanvasPicker : public QObject
{
    Q_OBJECT

public:
    explicit CanvasPicker(QwtPlot* plot);
    virtual bool eventFilter( QObject*, QEvent* ) override;

signals:
    void selected(int idx);

private:
    void select( const QPointF& );

    QwtPlot* plot();
    const QwtPlot* plot() const;

    QwtPlotCurve* m_selectedCurve;
    int m_selectedPoint;
};