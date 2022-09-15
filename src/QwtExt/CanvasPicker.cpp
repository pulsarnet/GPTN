//
// Created by darkp on 12.09.2022.
//

#include "CanvasPicker.h"
#include <QwtPlot>
#include <QwtPlotCurve>
#include <QMouseEvent>
#include <QwtGlobal>

CanvasPicker::CanvasPicker(QwtPlot *plot)
    : QObject(plot)
    , m_selectedPoint(-1)
{
    plot->canvas()->installEventFilter(this);
}

bool CanvasPicker::eventFilter(QObject *object, QEvent *event) {
    if (plot() == nullptr || object != plot()->canvas())
        return false;

    switch (event->type()) {
        case QEvent::MouseButtonDblClick:
        {
            const QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            select(mouseEvent->pos());
            return true;
        }
        default:
            break;
    }

    return QObject::eventFilter(object, event);
}

QwtPlot *CanvasPicker::plot() {
    return qobject_cast<class QwtPlot*>(parent());
}

const QwtPlot *CanvasPicker::plot() const {
    return qobject_cast<const QwtPlot*>(parent());
}

void CanvasPicker::select(const QPointF &pos) {

    double dist = 10e10;
    QwtPlotCurve* plotCurve = nullptr;
    int index = -1;

    auto& items = plot()->itemList();
    for (QwtPlotItemIterator it = items.begin(); it != items.end(); ++it) {
        if ((*it)->rtti() == QwtPlotItem::Rtti_PlotCurve) {
            auto curve = static_cast<QwtPlotCurve*>(*it);
            if (curve->isVisible()) {
                double d;
                int idx = curve->closestPoint(pos, &d);
                if (d < dist) {
                    plotCurve = curve;
                    dist = d;
                    index = idx;
                }
            }
        }
    }

    m_selectedCurve = nullptr;
    m_selectedPoint = -1;

    if (plotCurve && dist < 10) {
        m_selectedCurve = plotCurve;
        m_selectedPoint = index;
    }

    emit selected(m_selectedPoint);
}