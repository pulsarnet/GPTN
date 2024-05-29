#include "ReachabilityView.h"
#include "../../Editor/GraphicsViewZoom.h"

ReachabilityView::ReachabilityView(QWidget* parent)
    : QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setOptimizationFlags(DontAdjustForAntialiasing);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    m_zoom = QPointer(new GraphicsViewZoom(this));
    m_zoom->setModifier(Qt::NoModifier);
}

void ReachabilityView::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MiddleButton) {
        m_origin = event->pos();
        auto icon = QPixmap(":/images/tools/move.svg");
        setCursor(QCursor(icon));
        setInteractive(false);
    }
    QGraphicsView::mousePressEvent(event);
}

void ReachabilityView::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::MiddleButton) {
        QPointF oldp = mapToScene(m_origin.toPoint());
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        m_origin = event->pos();
    }
    QGraphicsView::mouseMoveEvent(event);
}

void ReachabilityView::mouseReleaseEvent(QMouseEvent *event) {
    setInteractive(true);
    setCursor(Qt::ArrowCursor);
    QGraphicsView::mouseReleaseEvent(event);
}
