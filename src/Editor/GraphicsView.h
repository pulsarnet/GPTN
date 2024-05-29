#ifndef FFI_RUST_GRAPHICSVIEW_H
#define FFI_RUST_GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
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

    SimulationWidget* simulationWidget() { return m_simulationWidget; }

protected:

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void contextMenu();

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
    QAction* m_directedAction = nullptr;
    QAction* m_inhibitorAction = nullptr;
    QAction* m_rotationAction = nullptr;
    QAction* m_removeAction = nullptr;
    QAction* m_markerAction = nullptr;
    QActionGroup* m_actionGroup = nullptr;

    SimulationWidget* m_simulationWidget = nullptr;

    MainWindow* m_mainWindow;

};


#endif //FFI_RUST_GRAPHICSVIEW_H