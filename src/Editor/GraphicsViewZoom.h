#ifndef FFI_RUST_GRAPHICSVIEWZOOM_H
#define FFI_RUST_GRAPHICSVIEWZOOM_H

#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsView>

class GraphicsViewZoom : public QObject {

    Q_OBJECT

public:
    explicit GraphicsViewZoom(QGraphicsView* view);

    void gentleZoom(double factor);

    void setModifier(Qt::KeyboardModifiers modifiers);

    void setZoomFactorBase(double value);

private:

    QGraphicsView* m_view;
    Qt::KeyboardModifiers m_modifiers;
    double m_zoom_factor_base;
    QPointF target_scene_pos, target_viewport_pos;

    bool eventFilter(QObject* object, QEvent* event);

signals:

    void zoomed();


};


#endif //FFI_RUST_GRAPHICSVIEWZOOM_H
