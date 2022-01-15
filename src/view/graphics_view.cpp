//
// Created by nmuravev on 12/13/2021.
//

#include <QFile>
#include "../../include/view/graphics_view.h"
#include "../../include/tab.h"

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ItemSelectionMode::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);

    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    zoom = new GraphicsViewZoom(this);
    zoom->set_modifier(Qt::NoModifier);

    setTransformationAnchor(QGraphicsView::NoAnchor);

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
    QGraphicsView::resizeEvent(event);
}

void GraphicsView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());
    painter.setFont(QFont());
    painter.drawText(rect().x() / 2., rect().y() / 2., rect().width(), 40, Qt::AlignCenter, this->windowTitle());
}