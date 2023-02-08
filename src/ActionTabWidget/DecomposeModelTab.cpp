//
// Created by nmuravev on 3/20/2022.
//

#include "DecomposeModelTab.h"
#include "../view/GraphicScene.h"
#include "../synthesis/synthesis_table.h"
#include "../DockSystem/DockToolbar.h"
#include <QGridLayout>
#include <QSplitter>
#include <DockAreaWidget.h>
#include <QLabel>
#include <unordered_map>

#include <Q3DScatter>
#include <QwtPickerMachine>
#include <QwtPlotGrid>
#include "../synthesis/synthesis_window.h"
#include "../DataVisualization/InputHandler3D.h"

uint qHash(const QVector3D &v)
{
    return qHash( QString( "%1x%2x%3" ).arg(v.x()).arg(v.y()).arg(v.z()) ) ;
}

DecomposeModelTab::DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent) : QWidget(parent)
    , m_netModelingTab(mainTab)
{
    mainTab->ctx()->decompose();
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

    Q3DScatter* scatter = new Q3DScatter();
    //scatter->setFlags(scatter->flags() ^ Qt::FramelessWindowHint);
    scatter->axisX()->setTitle("Positions");
    scatter->axisX()->setTitleVisible(true);

    scatter->axisY()->setTitle("Transitions");
    scatter->axisY()->setTitleVisible(true);

    scatter->axisZ()->setTitle("Edges");
    scatter->axisZ()->setTitleVisible(true);

    scatter->setAspectRatio(1.0f);
    scatter->setHorizontalAspectRatio(1.0f);
    scatter->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

    m_series = new QScatter3DSeries();
    QScatterDataArray data;

    connect(m_series, &QScatter3DSeries::selectedItemChanged, this, &DecomposeModelTab::selectedPoint);

    // Plot
    for(std::size_t i = 0; i < m_ctx->programs(); i++) {
        if (i % 1000000 == 0) {
            qDebug() << "Synthesised " << i << " programs of " << m_ctx->programs();
        }

        auto program = m_ctx->eval_program(i);

        //auto x_axis = program->input_positions() + 2 * program->output_positions();
        //auto y_axis = program->positions().size() + program->transitions().size();

        auto x_axis = program->positions().size();
        auto y_axis = program->transitions().size();

        if (x_axis == 0 && y_axis == 0) {
            continue;
        }

        auto connections = program->connections();
        int weight = 0;
        for (auto& connection : connections) {
            weight += connection->weight();
        }

        QVector3D point(x_axis, y_axis, weight);
        auto it = m_graphPoints.find(point);
        if (it == m_graphPoints.end()) {
            //qDebug() << "New point: " << point << " size: " << m_graphPoints.size();
            m_graphPoints.insert(point, QVector<std::size_t>{(std::size_t)i});
            data << QVector3D(x_axis, y_axis, weight);
        } else {
            it.value().push_back(i);
        }

        program->drop();
    }

    m_series->dataProxy()->addItems(data);
    scatter->addSeries(m_series);

    auto handler = new InputHandler3D();
    scatter->addInputHandler(handler);
    scatter->setActiveInputHandler(handler);

    m_plotWidget = new DockWidget("График");
    m_plotWidget->setWidget(QWidget::createWindowContainer(scatter));

    auto area = m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_linearBaseFragmentsView);
    area->setWindowTitle("Линейно-базовые фрагменты");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_dockManager->addDockWidget(ads::RightDockWidgetArea, m_primitiveNetView, area);
    area->setWindowTitle("Примитивная система");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_dockManager->addDockWidget(ads::BottomDockWidgetArea, m_plotWidget, area);
    area->setWindowTitle("График отношения");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setMouseTracking(true);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_dockManager);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void DecomposeModelTab::selectedPoint(int idx) {
    if (idx == -1)
        return;

    if (auto it = m_synthesisWindows.find((size_t)idx); it != m_synthesisWindows.end()) {
        auto window = it.value();
        window->show();
        window->activateWindow();
    } else {
        auto point = m_series->dataProxy()->itemAt(idx)->position();
        auto window = new SynthesisWindow(m_ctx, m_graphPoints[point]);
        m_synthesisWindows.insert(idx, window);
        window->show();
    }
}
