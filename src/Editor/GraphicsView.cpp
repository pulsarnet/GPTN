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

GraphicsView::GraphicsView(QWidget *parent) : QGraphicsView(parent) {

    setBackgroundBrush(Qt::NoBrush);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setWindowFlag(Qt::BypassGraphicsProxyWidget);
    this->setRubberBandSelectionMode(Qt::ContainsItemBoundingRect);
    this->setDragMode(QGraphicsView::RubberBandDrag);
    //this->setOptimizationFlags(DontAdjustForAntialiasing | DontSavePainterState);
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
        //setCursor(QCursor(Qt::SizeAllCursor));

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
    //m_toolBar->resizeEvent(event);
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
    //auto item = gScene->netItemAt(mapToScene(event->pos()));
    //if (!item) return;

    auto itemSelected = scene()->selectedItems().length() > 1;
    auto menu = new QMenu;
    menu->setWindowFlags(menu->windowFlags() | Qt::FramelessWindowHint);
    menu->setAttribute(Qt::WA_TranslucentBackground);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    auto horz = new QAction("Horizontal alignment", menu);
    horz->setEnabled(itemSelected);
    connect(horz, &QAction::triggered, gScene, &GraphicsScene::slotHorizontalAlignment);

    auto vert = new QAction("Vertical alignment", menu);
    vert->setEnabled(itemSelected);
    connect(vert, &QAction::triggered, gScene, &GraphicsScene::slotVerticalAlignment);

    auto matrix = new QAction("I/O matrix view", menu);
    connect(matrix, &QAction::triggered, this, &GraphicsView::slotMatrixView);

    auto reachability = new QAction("Reachability", menu);
    connect(reachability, &QAction::triggered, this, &GraphicsView::slotReachability);

    auto invariants = new QAction("Invariants", menu);
    connect(invariants, &QAction::triggered, this, &GraphicsView::slotInvariants);

    auto graphViz = new QMenu("GraphViz visualization", menu);

    auto dot = new QAction("dot", menu);
    auto neato = new QAction("neato", menu);
    auto twopi = new QAction("twopi", menu);
    auto circo = new QAction("circo", menu);
    auto fdp = new QAction("fdp", menu);
    auto osage = new QAction("osage", menu);
    auto patchwork = new QAction("patchwork", menu);
    auto sfdp = new QAction("sfdp", menu);


    connect(dot, &QAction::triggered, this, &GraphicsView::slotDotVisualization);
    connect(neato, &QAction::triggered, this, &GraphicsView::slotNeatoVisualization);
    connect(twopi, &QAction::triggered, this, &GraphicsView::slotTwopiVisualization);
    connect(circo, &QAction::triggered, this, &GraphicsView::slotCircoVisualization);
    connect(fdp, &QAction::triggered, this, &GraphicsView::slotFDPVisualization);
    connect(osage, &QAction::triggered, this, &GraphicsView::slotOsageVisualization);
    connect(patchwork, &QAction::triggered, this, &GraphicsView::slotPatchworkVisualization);
    connect(sfdp, &QAction::triggered, this, &GraphicsView::slotSFDPpVisualization);


    graphViz->addAction(dot);
    graphViz->addAction(neato);
    graphViz->addAction(twopi);
    graphViz->addAction(circo);
    graphViz->addAction(fdp);
    graphViz->addAction(osage);
    graphViz->addAction(patchwork);
    graphViz->addAction(sfdp);

    menu->addAction(horz);
    menu->addAction(vert);
    menu->addAction(matrix);
    menu->addAction(reachability);
    menu->addAction(invariants);
    menu->addMenu(graphViz);
    menu->popup(event->globalPos());

    QGraphicsView::contextMenuEvent(event);
}

void GraphicsView::slotMatrixView(bool checked) {
    Q_UNUSED(checked)

    if (m_IOMatrixWindow)
        m_IOMatrixWindow->activateWindow();
    else {
        auto matrix = dynamic_cast<GraphicsScene*>(scene())->net()->as_matrix();
        m_IOMatrixWindow = new MatrixWindow(matrix.first, matrix.second);
        connect(m_IOMatrixWindow, &MatrixWindow::onWindowClose, this, &GraphicsView::slotIOWindowClose);
        m_IOMatrixWindow->show();
    }
}

void GraphicsView::slotReachability(bool checked) {
    Q_UNUSED(checked)
    auto net = dynamic_cast<GraphicsScene*>(scene())->net();
    auto reachability = net->reachability();
    auto reachabilityWindow = new ReachabilityWindow(net, reachability);
    reachabilityWindow->show();
}

void GraphicsView::slotInvariants(bool checked) {
    Q_UNUSED(checked)
    auto net = dynamic_cast<GraphicsScene*>(scene())->net();
    qDebug() << "P/T-invariants: ";
    net->p_invariant();
    net->t_invariant();
}

void GraphicsView::slotIOWindowClose(QWidget *window) {
    qDebug() << "Remove widget: " << window;
    m_IOMatrixWindow = nullptr;
}

void GraphicsView::slotDotVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "dot");
}

void GraphicsView::slotNeatoVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "neato");
}

void GraphicsView::slotTwopiVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "twopi");
}

void GraphicsView::slotCircoVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "circo");
}

void GraphicsView::slotFDPVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "fdp");
}

void GraphicsView::slotOsageVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "osage");
}

void GraphicsView::slotPatchworkVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "patchwork");
}

void GraphicsView::slotSFDPpVisualization(bool checked) {
    Q_UNUSED(checked)
    dynamic_cast<GraphicsScene*>(scene())->dotVisualization((char *) "sfdp");
}

GraphicsView::~GraphicsView() noexcept {
    if (m_IOMatrixWindow)
        m_IOMatrixWindow->close();
}