//
// Created by nmuravev on 12/13/2021.
//

#include <QMenu>
#include <QActionGroup>

#include "Simulation/SimulationWidget.h"
#include "elements/position.h"
#include "elements/arrow_line.h"
#include "GraphicsView.h"
#include "GraphicsViewZoom.h"
#include "GraphicsScene.h"
#include "toolbox/toolbox.h"
#include "../overrides/MatrixWindow.h"
#include "../modules/reachability/reachability_window.h"
#include "../MainWindow.h"
#include "GraphicsSceneActions.h"

GraphicsView::GraphicsView(MainWindow* window, QWidget *parent)
    : QGraphicsView(parent)
    , m_mainWindow(window)
{
    setBackgroundBrush(Qt::NoBrush);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setCacheMode(CacheBackground);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    m_zoom = new GraphicsViewZoom(this);
    m_zoom->setModifier(Qt::NoModifier);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    m_mainToolBar = new ToolBox;
    m_mainToolBar->setParent(this);
    m_mainToolBar->setVisible(true);
    m_mainToolBar->setToolArea(ToolBox::TopLeft);
    m_mainToolBar->setButtonSize(QSize(40, 40));

    m_actionGroup = new QActionGroup(m_mainToolBar);
    m_positionAction = makeAction(QIcon(":/images/tools/position.svg"), tr("Position"), true, GraphicsScene::A_Position, m_actionGroup);
    m_transitionAction = makeAction(QIcon(":/images/tools/transition.svg"), tr("Transition"), true, GraphicsScene::A_Transition, m_actionGroup);
    m_moveAction = makeAction(QIcon(":/images/tools/move.svg"), tr("Move"), true, GraphicsScene::A_Move, m_actionGroup);
    m_connectAction = makeAction(QIcon(":/images/tools/connect.svg"), tr("Connect"), true, GraphicsScene::A_Connection, m_actionGroup);
    m_rotationAction = makeAction(QIcon(":/images/tools/rotation.svg"), tr("Rotate"), true, GraphicsScene::A_Rotation, m_actionGroup);
    m_removeAction = makeAction(QIcon(":/images/tools/remove.svg"), tr("Remove"), true, GraphicsScene::A_Remove, m_actionGroup);
    m_markerAction = makeAction(QIcon(":/images/tools/marker.svg"), tr("Marker"), true, GraphicsScene::A_Marker, m_actionGroup);
    connect(m_actionGroup, &QActionGroup::triggered, this, &GraphicsView::onToolBoxAction);

    m_mainToolBar->addTool(m_positionAction);
    m_mainToolBar->addTool(m_markerAction);
    m_mainToolBar->addTool(m_transitionAction);
    m_mainToolBar->addTool(m_connectAction);
    m_mainToolBar->addTool(m_removeAction);
    m_mainToolBar->addTool(m_moveAction);
    m_mainToolBar->addTool(m_rotationAction);

    m_simulationWidget = new SimulationWidget(this);
    auto geometry = m_simulationWidget->geometry();
    // centered bottom
    m_simulationWidget->setGeometry(
            (this->width() - geometry.width()) / 2,
            this->height() - geometry.height() - 5,
            geometry.width(),
            geometry.height()
            );

    setContentsMargins(0, 0, 0, 0);
}

void GraphicsView::setToolBoxVisibility(bool visible) {
    m_mainToolBar->setVisible(visible);
}

void GraphicsView::setAllowSimulation(bool allow) {
    m_simulationWidget->setVisible(allow);
}

QAction* GraphicsView::makeAction(const QIcon &icon, const QString &name, bool checkable, const QVariant& data_, QActionGroup *actionGroup_) {
    Q_ASSERT(actionGroup_);
    auto action = actionGroup_->addAction(icon, name);
    action->setCheckable(checkable);
    action->setData(data_);
    return action;
}

void GraphicsView::onToolBoxAction(QAction* action) {
    Q_UNUSED(action);
    auto lScene = dynamic_cast<GraphicsScene*>(scene());
    if (auto checked = m_actionGroup->checkedAction(); checked) {
        lScene->setMode((GraphicsScene::Mode)checked->data().toInt());
    } else {
        lScene->setMode(GraphicsScene::Mode::A_Nothing);
    }
}

void GraphicsView::mousePressEvent(QMouseEvent *event) {

    if (event->button() == Qt::MiddleButton) {
        m_origin = event->pos();
        auto icon = QPixmap(":/images/tools/move.svg");

        setCursor(QCursor(icon));
        setInteractive(false);
    }

    QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event) {

     if (event->buttons() & Qt::MiddleButton) {
        QPointF oldp = mapToScene(m_origin.toPoint());
        QPointF newp = mapToScene(event->pos());
        QPointF translation = newp - oldp;

        translate(translation.x(), translation.y());

        m_origin = event->pos();
    }

    QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event) {
    setInteractive(true);
    setCursor(Qt::ArrowCursor);

    QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::resizeEvent(QResizeEvent *event) {
    m_mainToolBar->resizeEvent(event);

    auto geometry = m_simulationWidget->geometry();
    // centered bottom
    m_simulationWidget->setGeometry(
            (rect().width() - geometry.width()) / 2,
            rect().height() - geometry.height() - 10,
            geometry.width(),
            geometry.height()
    );

    QGraphicsView::resizeEvent(event);
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event) {
    auto gScene = dynamic_cast<GraphicsScene*>(scene());

    QMenu menu;
    menu.deleteLater();
    menu.setWindowFlags(menu.windowFlags() | Qt::FramelessWindowHint);
    menu.setAttribute(Qt::WA_TranslucentBackground);
    menu.setAttribute(Qt::WA_DeleteOnClose);

    menu.addAction(gScene->actions()->hAlignmentAction());
    menu.addAction(gScene->actions()->vAlignmentAction());

    menu.addSeparator();

    menu.addMenu(gScene->actions()->graphViz());

    menu.exec(event->globalPos());

    QGraphicsView::contextMenuEvent(event);
}

GraphicsView::~GraphicsView() noexcept {
    if (m_IOMatrixWindow)
        m_IOMatrixWindow->close();
}