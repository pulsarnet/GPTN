//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_GRAPHICSVIEW_H
#define FFI_RUST_GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
#include <QTransform>
#include <QAction>

class ToolBox;
class GraphicsViewZoom;
class MatrixWindow;
class SimulationWidget;
class MainWindow;

class GraphicsView : public QGraphicsView {

    Q_OBJECT

public:

    explicit GraphicsView(MainWindow* window, QWidget* parent = nullptr);
    virtual ~GraphicsView() noexcept;

    void setToolBoxVisibility(bool visible = true);
    void setAllowSimulation(bool allow = true);

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

protected slots:

    void onToolBoxAction(QAction* action);

private:

    QAction* makeAction(const QIcon &icon, const QString &name, bool checkable, const QVariant& data, QActionGroup *actionGroup, const QKeySequence&);

    GraphicsViewZoom* m_zoom = nullptr;
    QPointF m_origin;
    ToolBox* m_mainToolBar = nullptr;

    QAction* m_positionAction = nullptr;
    QAction* m_transitionAction = nullptr;
    QAction* m_moveAction = nullptr;
    QAction* m_connectAction = nullptr;
    QAction* m_rotationAction = nullptr;
    QAction* m_removeAction = nullptr;
    QAction* m_markerAction = nullptr;
    QActionGroup* m_actionGroup = nullptr;

    MatrixWindow* m_IOMatrixWindow = nullptr;
    SimulationWidget* m_simulationWidget = nullptr;

    MainWindow* m_mainWindow;

};


#endif //FFI_RUST_GRAPHICSVIEW_H