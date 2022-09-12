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
#include "../QwtExt/qwt_ext_plot_curve_labels.h"
#include "../QwtExt/CanvasPicker.h"

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
    QHash<QPoint, QVector<std::size_t>> map;
    for(int i = 0; i < m_ctx->programs(); i++) {
        auto program = m_ctx->eval_program(i);

        auto x_axis = m_ctx->position_united(i);
        auto y_axis = program->connections().size();

        QPoint point = QPoint((int)x_axis, (int)y_axis);
        auto it = map.find(point);
        if (it == map.end()) {
            map.insert(point , QVector<std::size_t>({(std::size_t)i}));
        } else {
            (*it).push_back(i);
        }
    }

    auto symbol = new QwtSymbol(QwtSymbol::Ellipse,
                                QBrush( Qt::blue ), QPen( Qt::blue, 3 ), QSize( 9, 9 ) );

    m_plot = new QwtExtPlotCurveLabels;
    m_plot->setStyle(QwtPlotCurve::Dots);
    m_plot->setData(std::move(map));
    m_plot->setSymbol(symbol);
    m_plot->setRenderHint(QwtPlotItem::RenderAntialiased);

    auto qwt_plot = new QwtPlot;
    qwt_plot->setAxisTitle(QwtAxis::XBottom, "Position United");
    qwt_plot->setAxisTitle(QwtAxis::YLeft, "Connections");
    m_plot->attach(qwt_plot);

    auto grid = new QwtPlotGrid;
    grid->setMajorPen(QPen( Qt::gray, 0.5 ));
    grid->setRenderHint(QwtPlotItem::RenderAntialiased);
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

    auto canvasPicker = new CanvasPicker(qwt_plot);
    connect(canvasPicker, &CanvasPicker::selected, this, &DecomposeModelTab::selectedPoint);

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

void DecomposeModelTab::selectedPoint(int idx) {
    qDebug() << this->m_plot->getData(idx).length();
}
