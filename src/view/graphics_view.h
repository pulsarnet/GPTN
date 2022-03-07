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
#include <QAction>

class ToolBox;
class GraphicsViewZoom;
class MatrixWindow;

class GraphicsView : public QGraphicsView {

    Q_OBJECT

public:

    explicit GraphicsView(QWidget* parent = nullptr);
    virtual ~GraphicsView() noexcept;

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

    void positionChecked(bool checked);
    void transitionChecked(bool checked);
    void moveChecked(bool checked);
    void connectChecked(bool checked);
    void rotateChecked(bool checked);
    void removeChecked(bool checked);
    void markerChecked(bool checked);

    void slotMatrixView(bool checked);
    void slotIOWindowClose(QWidget*);

private:

    QAction* makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup);

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;
    ToolBox* m_toolBar = nullptr;
    ToolBox* m_mainToolBar = nullptr;

    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;
    QAction* connect_action = nullptr;
    QAction* rotation_action = nullptr;
    QAction* remove_action = nullptr;
    QAction* marker_action = nullptr;
    QActionGroup* actionGroup = nullptr;

    MatrixWindow* m_IOMatrixWindow = nullptr;

};


#endif //FFI_RUST_GRAPHICS_VIEW_H
