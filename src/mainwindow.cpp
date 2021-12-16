//
// Created by Николай Муравьев on 11.12.2021.
//

#include "../include/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    this->net = make();
    qDebug() << count(this->net);
    qDebug() << count(this->net);
    ::del(this->net);

    createToolBar();
    createMenuBar();
    createStatusBar();
    configureTab();

    connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);
    connect(this->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);


    this->setCentralWidget(this->tabWidget);
}

void MainWindow::tabChanged(int index) {
    this->scene = dynamic_cast<GraphicsView*>(tabWidget->widget(index));
}

void MainWindow::closeTab(int index) {
    this->tabWidget->removeTab(index);
}

void MainWindow::positionChecked(bool checked) {
    if (!scene) return;

    if (checked) scene->setAction(GraphicsView::A_Position);
    else scene->setAction(GraphicsView::A_Nothing);
}

void MainWindow::transitionChecked(bool checked) {
    if (!scene) return;

    if (checked) scene->setAction(GraphicsView::A_Transition);
    else scene->setAction(GraphicsView::A_Nothing);
}

void MainWindow::moveChecked(bool checked) {
    if (!scene) return;

    if (checked) scene->setAction(GraphicsView::A_Move);
    else scene->setAction(GraphicsView::A_Nothing);
}

void MainWindow::connectChecked(bool checked) {
    if (!scene) return;

    if (checked) scene->setAction(GraphicsView::A_Connect);
    else scene->setAction(GraphicsView::A_Nothing);
}

void MainWindow::rotateChecked(bool checked) {
    if (!scene) return;

    if(checked) scene->setAction(GraphicsView::A_Rotate);
    else scene->setAction(GraphicsView::A_Nothing);
}

void MainWindow::configureTab() {

    this->tabWidget = new QTabWidget;
    this->tabWidget->setTabShape(QTabWidget::Rounded);
    this->tabWidget->setTabsClosable(true);

    QToolButton *tb = new QToolButton();
    tb->setText("+");

    tabWidget->addTab(new QLabel("Add tabs by pressing \"+\""), QString());
    tabWidget->setTabEnabled(0, false);

    tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, tb);

    connect(tb, &QPushButton::released, this, &MainWindow::newTab);
}

void MainWindow::newTab() {
    auto index = this->tabWidget->addTab(new GraphicsView, "New Tab");
    scene = dynamic_cast<GraphicsView*>(this->tabWidget->widget(this->tabWidget->currentIndex()));
    this->tabWidget->setCurrentIndex(index);
}

QAction *MainWindow::makeAction(const QString &name, const QIcon &icon, bool checkable, QActionGroup *actionGroup) {
    auto action = new QAction(name);
    action->setIcon(icon);
    action->setCheckable(checkable);

    if (actionGroup) actionGroup->addAction(action);

    return action;
}

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar;
    //menuBar->addAction(move_action);
    this->setMenuBar(menuBar);
}

void MainWindow::createStatusBar() {
    statusBar = new QStatusBar;
    this->setStatusBar(statusBar);
}

void MainWindow::createToolBar() {
    toolBar = new QToolBar;
    this->addToolBar(Qt::LeftToolBarArea, toolBar);

    QActionGroup* actionGroup = new QActionGroup(toolBar);

    position_action = makeAction("Position", QIcon(":/images/circle.png"), true, actionGroup);
    transition_action = makeAction("Transition", QIcon(":/images/rectangle.png"), true, actionGroup);
    move_action = makeAction("Move", QIcon(":/images/move.png"), true, actionGroup);
    connect_action = makeAction("Connect", QIcon(":/images/connect.png"), true, actionGroup);
    rotation_action = makeAction("Rotate", QIcon(":/images/rotation.png"), true, actionGroup);


    connect(position_action, &QAction::toggled, this, &MainWindow::positionChecked);
    connect(transition_action, &QAction::toggled, this, &MainWindow::transitionChecked);
    connect(move_action, &QAction::toggled, this, &MainWindow::moveChecked);
    connect(connect_action, &QAction::toggled, this, &MainWindow::connectChecked);
    connect(rotation_action, &QAction::toggled, this, &MainWindow::rotateChecked);


    toolBar->addAction(position_action);
    toolBar->addAction(transition_action);
    toolBar->addAction(move_action);
    toolBar->addAction(connect_action);
    toolBar->addAction(rotation_action);
}
