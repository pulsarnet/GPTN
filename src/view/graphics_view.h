//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_GRAPHICS_VIEW_H
#define FFI_RUST_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
#include <QTransform>

class ToolBox;
class GraphicsViewZoom;

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

    void contextMenuEvent(QContextMenuEvent *event) override;

protected slots:

    void slotDotVisualization(bool checked);
    void slotNeatoVisualization(bool checked);
    void slotTwopiVisualization(bool checked);
    void slotCircoVisualization(bool checked);
    void slotFDPVisualization(bool checked);
    void slotOsageVisualization(bool checked);
    void slotPatchworkVisualization(bool checked);
    void slotSFDPpVisualization(bool checked);

signals:

private:

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;
    ToolBox* m_toolBar = nullptr;

};


#endif //FFI_RUST_GRAPHICS_VIEW_H
