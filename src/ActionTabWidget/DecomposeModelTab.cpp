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
#include <QChart>
#include <QScatterSeries>
#include <QChartView>
#include <unordered_map>
#include <QValueAxis>

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

    auto lineSeries = new QScatterSeries;
    lineSeries->setPointLabelsFormat("(@xPoint, @yPoint)");
    lineSeries->setPointLabelsVisible(true);

    QHash<QPoint, std::size_t> map;
    for(int i = 0; i < m_ctx->programs(); i++) {
        auto program = m_ctx->eval_program(i);
        QPoint point = QPoint(program->positions().size(), program->connections().size());
        auto it = map.find(point);
        if (it == map.end()) {
            map.insert(point , 1);
        } else {
            *it += 1;
        }
    }

    for (auto i = map.begin(); i != map.end(); ++i) {
        lineSeries->append(i.key());
    }

    auto xAxis = new QValueAxis;
    xAxis->setRange(0, 10);
    xAxis->setLabelFormat("%d");
    xAxis->setTickCount(7);
    xAxis->setTitleText("Positions");

    auto yAxis = new QValueAxis;
    yAxis->setRange(0, 10);
    yAxis->setLabelFormat("%d");
    yAxis->setTickCount(7);
    yAxis->setTitleText("Connections");

    auto chart = new QChart;
    chart->legend()->hide();
    chart->addSeries(lineSeries);

    chart->addAxis(xAxis, Qt::AlignBottom);
    chart->addAxis(yAxis, Qt::AlignLeft);

    lineSeries->attachAxis(xAxis);
    lineSeries->attachAxis(yAxis);

    auto plotWidget = new QChartView(chart);
    plotWidget->setRenderHint(QPainter::Antialiasing);
    m_plotWidget = new DockWidget("Plot widget");
    m_plotWidget->setWidget(plotWidget);

    auto area = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_linearBaseFragmentsView);
    area->setWindowTitle("Линейно-базовые фрагменты");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_primitiveNetView, area);
    area->setWindowTitle("Примитивная система");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_plotWidget, area);
    area->setWindowTitle("График отношения");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setMouseTracking(true);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_dockManager);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void DecomposeModelTab::wheelEvent(QWheelEvent *event) {
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        qreal factor = event->angleDelta().y() < 0 ? -2: 2;
        dynamic_cast<QChartView*>(m_plotWidget->widget())->chart()->scroll(factor, 0);
    } else if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        qreal factor = event->angleDelta().y() < 0 ? -2: 2;
        dynamic_cast<QChartView*>(m_plotWidget->widget())->chart()->scroll(0, factor);
    } else {
        qreal factor = event->angleDelta().y() < 0 ? 0.75: 1.5;
        dynamic_cast<QChartView*>(m_plotWidget->widget())->chart()->zoom(factor);
    }

    event->accept();
}