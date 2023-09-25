//
// Created by nmuravev on 12/15/2021.
//

#include <QtMath>
#include <QApplication>
#include "GraphicsViewZoom.h"

GraphicsViewZoom::GraphicsViewZoom(QGraphicsView *view) : QObject(view), m_view(view) {
    m_view->viewport()->installEventFilter(this);
    m_view->setMouseTracking(true);
    m_modifiers = Qt::ControlModifier;
    m_zoom_factor_base = 1.0015;
}

void GraphicsViewZoom::gentleZoom(double factor) {
    m_view->scale(factor, factor);
    m_view->centerOn(target_scene_pos);

    QPointF delta_viewport_pos = target_viewport_pos - QPointF(m_view->viewport()->width() / 2., m_view->viewport()->height() / 2.);
    QPointF viewport_center = m_view->mapFromScene(target_scene_pos) - delta_viewport_pos;
    m_view->centerOn(m_view->mapToScene(viewport_center.toPoint()));
    emit zoomed();
}

void GraphicsViewZoom::setModifier(Qt::KeyboardModifiers modifiers) {
    m_modifiers = modifiers;
}

void GraphicsViewZoom::setZoomFactorBase(double value) {
    m_zoom_factor_base = value;
}

bool GraphicsViewZoom::eventFilter(QObject *object, QEvent *event) {
    Q_UNUSED(object);
    if (event->type() == QEvent::MouseMove) {
        auto mouse_event = dynamic_cast<QMouseEvent*>(event);
        QPointF delta = target_viewport_pos - mouse_event->pos();
        if (qAbs(delta.x()) > 5 || qAbs(delta.y()) > 5) {
            target_viewport_pos = mouse_event->pos();
            target_scene_pos = m_view->mapToScene(mouse_event->pos());
        }
    }
    else if (event->type() == QEvent::Wheel) {
        auto wheel_event = dynamic_cast<QWheelEvent*>(event);
        if (QApplication::keyboardModifiers() == m_modifiers && wheel_event->angleDelta().y() != 0) {
            double angle = wheel_event->angleDelta().y();
            double factor = qPow(m_zoom_factor_base, angle);
            gentleZoom(factor);
            return true;
        }
    }
    else if (event->type() == QEvent::NativeGesture) {
        auto nge = dynamic_cast<QNativeGestureEvent*>(event);
        if (nge) {
            if (nge->gestureType() == Qt::ZoomNativeGesture) {
                double angle = nge->delta().y();
                double factor = qPow(m_zoom_factor_base, angle);
                gentleZoom(factor);
                return true;
            }
        }
    }
    return false;
}
