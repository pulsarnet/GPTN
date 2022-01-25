//
// Created by nmuravev on 12/13/2021.
//

#include <QMenu>

#include "../elements/position.h"
#include "../elements/arrow_line.h"
#include "graphics_view.h"
#include "graphics_view_zoom.h"
#include "graphic_scene.h"
#include "../toolbox/toolbox.h"
#include "../graphviz/graphviz_wrapper.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);

    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    zoom = new GraphicsViewZoom(this);
    zoom->set_modifier(Qt::NoModifier);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    m_toolBar = new ToolBox;
    m_toolBar->setVisible(true);
    m_toolBar->setParent(this);
    m_toolBar->setToolArea(ToolBox::TopRight);
    m_toolBar->setButtonSize(QSize(20, 20));

    auto graphViz = new QMenu("GraphViz visualization");
    m_toolBar->addTool(graphViz);

    auto dot = new QAction("dot algorithm");
    auto neato = new QAction("neato algorithm");
    auto twopi = new QAction("twopi algorithm");
    auto circo = new QAction("circo algorithm");
    auto fdp = new QAction("fdp algorithm");
    auto osage = new QAction("osage algorithm");
    auto patchwork = new QAction("patchwork algorithm");
    auto sfdp = new QAction("sfdp algorithm");


    connect(dot, &QAction::triggered, this, &GraphicsView::slotDotVisualization);
    connect(neato, &QAction::triggered, this, &GraphicsView::slotNeatoVisualization);
    connect(twopi, &QAction::triggered, this, &GraphicsView::slotTwopiVisualization);
    connect(circo, &QAction::triggered, this, &GraphicsView::slotCircoVisualization);
    connect(fdp, &QAction::triggered, this, &GraphicsView::slotFDPVisualization);
    connect(osage, &QAction::triggered, this, &GraphicsView::slotOsageVisualization);
    connect(patchwork, &QAction::triggered, this, &GraphicsView::slotPatchworkVisualization);
    connect(sfdp, &QAction::triggered, this, &GraphicsView::slotSFDPpVisualization);


    graphViz->addAction(dot);
    graphViz->addAction(neato);
    graphViz->addAction(twopi);
    graphViz->addAction(circo);
    graphViz->addAction(fdp);
    graphViz->addAction(osage);
    graphViz->addAction(patchwork);
    graphViz->addAction(sfdp);


}

void GraphicsView::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::MiddleButton) {
        m_origin = event->pos();
        setCursor(QCursor(Qt::SizeAllCursor));
        setInteractive(false);
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {

     if (event->buttons() & Qt::MiddleButton) {
        QPointF oldp = mapToScene(m_origin.toPoint());
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        m_origin = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    setInteractive(true);
    setCursor(Qt::ArrowCursor);

    QGraphicsView::mouseReleaseEvent(event);
}


void GraphicsView::resizeEvent(QResizeEvent *event) {
    m_toolBar->resizeEvent(event);
    QGraphicsView::resizeEvent(event);
}

void GraphicsView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());
    painter.setFont(QFont());
    painter.drawText(rect().x() / 2., rect().y() / 2., rect().width(), 40, Qt::AlignCenter, this->windowTitle());
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    auto gScene = dynamic_cast<GraphicScene*>(scene());
    auto item = gScene->netItemAt(mapToScene(event->pos()));
    if (!item) return;

    if (item->isSelected() && scene()->selectedItems().length() > 1) {
        QMenu *menu = new QMenu;

        auto horz = new QAction("Horizontal alignment", this);
        connect(horz, &QAction::triggered, gScene, &GraphicScene::slotHorizontalAlignment);

        auto vert = new QAction("Vertical alignment", this);
        connect(vert, &QAction::triggered, gScene, &GraphicScene::slotVerticalAlignment);

        menu->addAction(horz);
        menu->addAction(vert);
        menu->popup(event->globalPos());
    }


    QGraphicsView::contextMenuEvent(event);
}

void GraphicsView::slotDotVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "dot");
}

void GraphicsView::slotNeatoVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "neato");
}

void GraphicsView::slotTwopiVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "twopi");
}

void GraphicsView::slotCircoVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "circo");
}

void GraphicsView::slotFDPVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "fdp");
}

void GraphicsView::slotOsageVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "osage");
}

void GraphicsView::slotPatchworkVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "patchwork");
}

void GraphicsView::slotSFDPpVisualization(bool checked) {
    dynamic_cast<GraphicScene*>(scene())->dotVisualization((char *) "sfdp");
}