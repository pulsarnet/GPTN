//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_GRAPHICS_VIEW_H
#define FFI_RUST_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
#include <QTransform>
#include "graphics_view_zoom.h"
#include "../elements/position.h"
#include "../elements/arrow_line.h"
#include "../elements/petri_object.h"

class GraphicsView : public QGraphicsView {

    Q_OBJECT

public:

    explicit GraphicsView(QWidget* parent = nullptr);

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

protected slots:

signals:

private:

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;

};


#endif //FFI_RUST_GRAPHICS_VIEW_H
