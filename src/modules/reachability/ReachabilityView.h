#ifndef FFI_RUST_REACHABILITY_VIEW_H
#define FFI_RUST_REACHABILITY_VIEW_H

#include <QGraphicsView>
#include <QPointer>

class GraphicsViewZoom;

class ReachabilityView: public QGraphicsView {

public:

    explicit ReachabilityView(QWidget* parent = nullptr);

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    QPointF m_origin;
    QPointer<GraphicsViewZoom> m_zoom;
};


#endif //FFI_RUST_REACHABILITY_VIEW_H
