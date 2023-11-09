//
// Created by darkp on 22.10.2022.
//

#include "QwtTimeLineTransitionActivity.h"
#include "../../Core/FFI/simulation.h"
#include "QwtVertexScaleDraw.h"
#include <QTimer>
#include <QwtPlotShapeItem>
#include <QPainterPath>
#include <QPen>
#include <QwtPlotGrid>
#include <QwtPlotLegendItem>

QwtTimeLineTransitionActivity::QwtTimeLineTransitionActivity(QWidget* parent)
    : QwtPlot(parent)
    , m_simulation(nullptr)
    , m_timer(new QTimer(this))
    , m_cycle(0)
    , m_updated(false)
{
    this->enableAxis(yLeft, true); // transitions
    this->enableAxis(xTop, true); // timeline
    this->enableAxis(xBottom, false);
    this->enableAxis(yRight, false);

    this->setMinimumHeight(100);
    this->setMinimumWidth(400);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    this->setAxisScaleDraw(xTop, new QwtScaleDraw);
    this->setAxisScaleDraw(yLeft, new QwtVertexScaleDraw);

    this->setFrameShape(NoFrame);
    this->setFrameShadow(Raised);

    this->canvas()->setAttribute(Qt::WA_StyledBackground);

    const auto grid = new QwtPlotGrid;
    grid->enableY(true);
    grid->enableX(false);
    grid->setMajorPen(QPen(Qt::gray, 0, Qt::DashLine));
    grid->attach(this);

    const auto legend = new QwtPlotLegendItem;
    legend->setSpacing(1);
    legend->attach(this);

    connect(m_timer,
            &QTimer::timeout,
            this,
            &QwtTimeLineTransitionActivity::updatePlot);

    resetPlot();
}

void QwtTimeLineTransitionActivity::setSimulation(ffi::Simulation *simulation) {
    m_simulation = simulation;
    resetPlot();
}

void QwtTimeLineTransitionActivity::updatePlot() {
    static auto getStepLambda = [](const int count) {
        const auto number = QString::number(count);
        const auto length = number.length() - 1;
        return pow(10, length);
    };

    if (m_updated) {
        m_updated = false;
        this->setAxisScale(xTop, 0, m_cycle, getStepLambda(m_cycle));
        replot();
    }
}

void QwtTimeLineTransitionActivity::resetPlot() {
    // stop timer
    m_timer->stop();

    // clear plot
    m_shapes.clear();
    m_plotMapper.clear();
    this->detachItems(QwtPlotItem::Rtti_PlotShape, true);

    // for each transition in simulation create shape
    if (m_simulation) {
        int counter = 1;
        auto transitions = m_simulation->net()->transitions();
        for (const auto transition : transitions) {
            const auto shape = new QwtPlotShapeItem();
            shape->setPen(QPen(Qt::black, 1, Qt::SolidLine));
            shape->setBrush(QBrush(QColor("#349eeb")));
            shape->attach(this);
            m_shapes[transition->index()] = shape;

            // map
            m_plotMapper[transition->index()] = counter++;
        }

        // set axis scale labels
        QHash<int, QString> labels;
        for (const auto transition : transitions) {
            labels[m_plotMapper[transition->index()]] = QString("T%1").arg(transition->index().id);
        }

        const auto scaleDraw = dynamic_cast<QwtVertexScaleDraw*>(this->axisScaleDraw(yLeft));
        scaleDraw->setData(labels);
    }

    // reset scale
    this->setAxisScale(xTop, 0, 1, 1);
    this->setAxisScale(yLeft, 1, (double)m_shapes.size() + 1., 1);

    // start timer
    if (m_simulation) {
        m_timer->start(1000);
    }

    replot();
}

void QwtTimeLineTransitionActivity::registerFire(ffi::VertexIndex index, int cycle) {
    // Получить item
    auto item = m_shapes.find(index);
    if (item == m_shapes.end()) {
        qDebug() << "Item not found";
        return;
    }

    auto transition = item.value();
    auto shape = transition->shape();
    shape.addRect(cycle - 1, m_plotMapper[index], 1, 1);
    transition->setShape(shape);

    m_cycle = std::max(m_cycle, cycle);
    m_updated = true;
}
