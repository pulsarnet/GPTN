//
// Created by Николай Муравьев on 11.12.2021.
//

#include "../include/mainwindow.h"
#include "../include/tab.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    this->net = make();
    qDebug() << count(this->net);
    qDebug() << count(this->net);
    ::del(this->net);

    createToolBar();
    createMenuBar();
    createStatusBar();
    configureTab();

    connect(this->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    this->setCentralWidget(this->tabWidget);

    tabChanged(-1);
}

void MainWindow::closeTab(int index) {
    this->tabWidget->removeTab(index);
}

void MainWindow::positionChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Position : GraphicsView::A_Nothing);
    }
}

void MainWindow::transitionChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Transition : GraphicsView::A_Nothing);
    }
}

void MainWindow::moveChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Move : GraphicsView::A_Nothing);
    }
}

void MainWindow::connectChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Connect : GraphicsView::A_Nothing);
    }
}

void MainWindow::rotateChecked(bool checked) {
    if (auto scene = currentScene(); scene) {
        scene->setAction(checked ? GraphicsView::A_Rotate : GraphicsView::A_Nothing);
    }
}

void MainWindow::configureTab() {

    this->tabWidget = new QTabWidget;
    this->tabWidget->setTabShape(QTabWidget::Rounded);
    this->tabWidget->setTabsClosable(true);

}

void MainWindow::newTab() {
    auto index = tabWidget->addTab(new Tab, "New Tab");
    if (this->tabWidget->currentIndex() == 0) this->tabWidget->setCurrentIndex(index);
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

    auto file_menu = new QMenu("&File");
    {
        auto newMenu = new QMenu("New");

        auto new_action = new QAction("File");
        new_action->setShortcut(QKeySequence::fromString(tr("Ctrl+N")));
        newMenu->addAction(new_action);
        connect(new_action, &QAction::triggered, this, &MainWindow::newFile);

        file_menu->addMenu(newMenu);
    }

    file_menu->addAction(new QAction("&Open"));
    file_menu->addAction(new QAction("&Save"));
    file_menu->addAction(new QAction("&SaveAs"));

    menuBar->addMenu(file_menu);

    this->setMenuBar(menuBar);
}

void MainWindow::createStatusBar() {
    statusBar = new QStatusBar;
    this->setStatusBar(statusBar);
}

void MainWindow::createToolBar() {
    toolBar = new QToolBar;
    this->addToolBar(Qt::LeftToolBarArea, toolBar);

    actionGroup = new QActionGroup(toolBar);

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

GraphicsView *MainWindow::currentScene() {
    if (auto tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex())); tab) {
        return tab->scene();
    }

    return nullptr;
}

void MainWindow::newFile(bool trigger) {
    Q_UNUSED(trigger);
    this->newTab();
}

void MainWindow::tabChanged(int index) {
    auto checked = this->actionGroup->checkedAction();

    if (index == -1) {
        if (checked) checked->toggle();
        this->toolBar->setEnabled(false);
        return;
    }

    this->toolBar->setEnabled(true);

    auto currentAction = currentScene()->currentAction();

    if (checked) checked->toggle();

    switch (currentAction) {
        case GraphicsView::A_Position:
            position_action->toggle();
            break;
        case GraphicsView::A_Transition:
            transition_action->toggle();
            break;
        case GraphicsView::A_Connect:
            connect_action->toggle();
            break;
        case GraphicsView::A_Move:
            move_action->toggle();
            break;
        case GraphicsView::A_Rotate:
            rotation_action->toggle();
            break;
        case GraphicsView::A_Nothing:
            break;
    }
}
