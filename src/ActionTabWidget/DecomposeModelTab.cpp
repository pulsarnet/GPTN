//
// Created by nmuravev on 3/20/2022.
//

#include "DecomposeModelTab.h"
#include "../view/graphic_scene.h"
#include "../synthesis/synthesis_table.h"
#include "../DockSystem/DockToolbar.h"
#include <QGridLayout>
#include <QSplitter>
#include <DockAreaWidget.h>
#include <QLabel>
#include <unordered_map>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_picker.h>
#include <qwt_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_grid.h>

DecomposeModelTab::DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent) : QWidget(parent)
    , m_netModelingTab(mainTab)
{
    m_ctx = mainTab->ctx()->decompose_ctx();

    m_dockManager = new ads::CDockManager(this);

    auto linearBaseFragmentsScene = new GraphicScene(m_ctx->linear_base_fragments());
    auto linearBaseFragmentsView = new GraphicsView;
    linearBaseFragmentsView->setScene(linearBaseFragmentsScene);
    linearBaseFragmentsView->setToolBoxVisibility(false);
    m_linearBaseFragmentsView = new DockWidget("LBF");
    m_linearBaseFragmentsView->setWidget(linearBaseFragmentsView);

    auto primitiveNetScene = new GraphicScene(m_ctx->primitive_net());
    auto primitiveNetView = new GraphicsView;
    primitiveNetView->setScene(primitiveNetScene);
    primitiveNetView->setToolBoxVisibility(false);
    m_primitiveNetView = new DockWidget("Primitive view");
    m_primitiveNetView->setWidget(primitiveNetView);


    // Plot
    QHash<QPoint, std::size_t> map;
    for(int i = 0; i < m_ctx->programs(); i++) {
        auto program = m_ctx->eval_program(i);

        auto positions = program->positions();
        auto parents = 0;
        for (int i = 0; i < positions.size(); i++) {
            if (positions[i]->parent() != 0) {
                parents += 1;
            }
        }

        QPoint point = QPoint(positions.size(), parents);
        auto it = map.find(point);
        if (it == map.end()) {
            map.insert(point , 1);
        } else {
            *it += 1;
        }
    }

    auto keys = map.keys();
    QVector<QPointF> points;
    for (auto p : keys) {
        points.push_back(QPointF(p));
    }

    auto symbol = new QwtSymbol(QwtSymbol::Ellipse,
                                QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );

    auto qwt_chart = new QwtPlotCurve;
    qwt_chart->setStyle(QwtPlotCurve::Dots);
    qwt_chart->setSamples(points);
    qwt_chart->setSymbol(symbol);
    qwt_chart->setRenderHint(QwtPlotItem::RenderAntialiased);

    auto qwt_plot = new QwtPlot;
    qwt_plot->setAxisTitle(QwtAxis::XBottom, "Positions");
    qwt_plot->setAxisTitle(QwtAxis::YLeft, "Cut positions");
    qwt_chart->attach(qwt_plot);

    auto grid = new QwtPlotGrid;
    grid->setMajorPen(QPen( Qt::gray, 2 ));
    grid->attach( qwt_plot );

    auto magnifier = new QwtPlotMagnifier(qwt_plot->canvas());
    magnifier->setMouseButton(Qt::MiddleButton);

    auto panner = new QwtPanner(qwt_plot->canvas());
    panner->setMouseButton(Qt::RightButton);

    auto picker = new QwtPlotPicker(QwtAxis::XBottom,
                                    QwtAxis::YLeft,
                                    QwtPicker::CrossRubberBand,
                                    QwtPicker::AlwaysOn,
                                    qwt_plot->canvas());
    picker->setRubberBandPen(QColor(Qt::red));
    picker->setTrackerPen(QColor(Qt::black));
    picker->setStateMachine(new QwtPickerDragRectMachine());

    auto qwt_view = new DockWidget("График");
    qwt_view->setWidget(qwt_plot);

    auto area = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_linearBaseFragmentsView);
    area->setWindowTitle("Линейно-базовые фрагменты");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_primitiveNetView, area);
    area->setWindowTitle("Примитивная система");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_dockManager->addDockWidget(ads::BottomDockWidgetArea, qwt_view, area);
    area->setWindowTitle("График отношения");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setMouseTracking(true);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_dockManager);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void DecomposeModelTab::wheelEvent(QWheelEvent *event) {
//    if (event->modifiers().testFlag(Qt::ControlModifier)) {
//        qreal factor = event->angleDelta().y() < 0 ? -2: 2;
//        dynamic_cast<QChartView*>(m_plotWidget->widget())->chart()->scroll(factor, 0);
//    } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
//        qreal factor = event->angleDelta().y() < 0 ? -2: 2;
//        dynamic_cast<QChartView*>(m_plotWidget->widget())->chart()->scroll(0, factor);
//    } else {
//        qreal factor = event->angleDelta().y() < 0 ? 0.75: 1.5;
//        dynamic_cast<QChartView*>(m_plotWidget->widget())->chart()->zoom(factor);
//    }

    event->accept();
}