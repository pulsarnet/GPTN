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
    void paintEvent(QPaintEvent *event) override;

    void contextMenuEvent(QContextMenuEvent *event) override;

protected slots:

    void positionChecked(bool checked);
    void transitionChecked(bool checked);
    void moveChecked(bool checked);
    void connectChecked(bool checked);
    void rotateChecked(bool checked);
    void removeChecked(bool checked);
    void markerChecked(bool checked);

private:

    QAction* makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup);

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;
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
    SimulationWidget* m_simulationWidget = nullptr;

    MainWindow* m_mainWindow;

};


#endif //FFI_RUST_GRAPHICSVIEW_H