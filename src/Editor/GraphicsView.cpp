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
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    this->setCacheMode(CacheBackground);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    zoom = new GraphicsViewZoom(this);
    zoom->set_modifier(Qt::NoModifier);

    setTransformationAnchor(QGraphicsView::NoAnchor);

    m_mainToolBar = new ToolBox;
    m_mainToolBar->setParent(this);
    m_mainToolBar->setVisible(true);
    m_mainToolBar->setToolArea(ToolBox::TopLeft);
    m_mainToolBar->setButtonSize(QSize(40, 40));

    actionGroup = new QActionGroup(m_mainToolBar);

    position_action = makeAction("Position", QIcon(":/images/tools/position.svg"), true, actionGroup);
    transition_action = makeAction("Transition", QIcon(":/images/tools/transition.svg"), true, actionGroup);
    move_action = makeAction("Move", QIcon(":/images/tools/move.svg"), true, actionGroup);
    connect_action = makeAction("Connect", QIcon(":/images/tools/connect.svg"), true, actionGroup);
    rotation_action = makeAction("Rotate", QIcon(":/images/tools/rotation.svg"), true, actionGroup);
    remove_action = makeAction("Remove", QIcon(":/images/tools/remove.svg"), true, actionGroup);
    marker_action = makeAction("Marker", QIcon(":/images/tools/marker.svg"), true, actionGroup);


    connect(position_action, &QAction::toggled, this, &GraphicsView::positionChecked);
    connect(transition_action, &QAction::toggled, this, &GraphicsView::transitionChecked);
    connect(move_action, &QAction::toggled, this, &GraphicsView::moveChecked);
    connect(connect_action, &QAction::toggled, this, &GraphicsView::connectChecked);
    connect(rotation_action, &QAction::toggled, this, &GraphicsView::rotateChecked);
    connect(remove_action, &QAction::toggled, this, &GraphicsView::removeChecked);
    connect(marker_action, &QAction::toggled, this, &GraphicsView::markerChecked);


    m_mainToolBar->addTool(position_action);
    m_mainToolBar->addTool(marker_action);
    m_mainToolBar->addTool(transition_action);
    m_mainToolBar->addTool(connect_action);
    m_mainToolBar->addTool(remove_action);
    m_mainToolBar->addTool(move_action);
    m_mainToolBar->addTool(rotation_action);

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

QAction* GraphicsView::makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup_) {
    auto action = new QAction(name);
    action->setIcon(icon);
    action->setCheckable(checkable);

    if (actionGroup_) actionGroup_->addAction(action);

    return action;
}

void GraphicsView::positionChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Position : GraphicsScene::A_Nothing);
}

void GraphicsView::transitionChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Transition : GraphicsScene::A_Nothing);
}

void GraphicsView::moveChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Move : GraphicsScene::A_Nothing);
}

void GraphicsView::connectChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Connection : GraphicsScene::A_Nothing);
}

void GraphicsView::rotateChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Rotation : GraphicsScene::A_Nothing);
}

void GraphicsView::removeChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Remove : GraphicsScene::A_Nothing);
}

void GraphicsView::markerChecked(bool checked) {
    dynamic_cast<GraphicsScene*>(scene())->setMode(checked ? GraphicsScene::A_Marker : GraphicsScene::A_Nothing);
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

void GraphicsView::paintEvent(QPaintEvent *event) {
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());
    painter.setFont(QFont());
    painter.drawText(int(rect().x() / 2.), int(rect().y() / 2.), rect().width(), 40, Qt::AlignCenter, this->windowTitle());
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