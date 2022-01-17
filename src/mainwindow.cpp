//
// Created by Николай Муравьев on 11.12.2021.
//

#include "../include/mainwindow.h"


#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include "../include/matrix_model.h"
#include "../include/synthesis/synthesis_program_item_delegate.h"
#include <QTableView>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    createMenuBar();
    createStatusBar();
    configureTab();

    connect(this->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(this->tabWidget, &QTabWidget::currentChanged, this, &MainWindow::tabChanged);

    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, false);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);

    manager = new CDockManager(this);
    this->setCentralWidget(manager);

    auto centralDockWidget = new CDockWidget("Central");
    centralDockWidget->setWidget(this->tabWidget);
    manager->setCentralWidget(centralDockWidget);

    auto right = new CDockWidget("Whos");
    right->setWidget(new QTableView);
    manager->addDockWidget(DockWidgetArea::RightDockWidgetArea, right);

    tabChanged(-1);

}

void MainWindow::closeTab(int index) {
    auto tab = dynamic_cast<Tab*>(this->tabWidget->widget(index));
    tab->saveToFile();
    this->tabWidget->removeTab(index);
}

void MainWindow::configureTab() {
    this->tabWidget = new QTabWidget;
    this->tabWidget->setDocumentMode(false);
    this->tabWidget->setTabsClosable(true);

    auto tool = new QToolButton(tabWidget);
    tool->setPopupMode(QToolButton::InstantPopup);
    tool->setText("View");
    this->tabWidget->setCornerWidget(tool);
}

Tab* MainWindow::newTab() {
    auto tab = new Tab(this);
    auto index = tabWidget->addTab(tab, "New Tab");
    this->tabWidget->setCurrentIndex(index);
    updateTabViewMenu();
    return tab;
}

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar;

    auto file_menu = new QMenu("&File");
    {
        auto newMenu = new QMenu("New");

        auto new_action = new QAction("File");
        new_action->setShortcut(QKeySequence::fromString(tr("Ctrl+N")));
        newMenu->addAction(new_action);
        connect(new_action, &QAction::triggered, this, &MainWindow::newFile);

        file_menu->addMenu(newMenu);
    }

    auto save_action = new QAction("Save");
    save_action->setShortcut(tr("Ctrl+S"));
    connect(save_action, &QAction::triggered, this, &MainWindow::slotSaveFile);

    auto save_as_action = new QAction("Save as...");
    save_as_action->setShortcut(tr("Ctrl+Shift+S"));
    connect(save_as_action, &QAction::triggered, this, &MainWindow::slotSaveAsFile);

    auto open_action = new QAction("Open");
    open_action->setShortcut(tr("Ctrl+O"));
    connect(open_action, &QAction::triggered, this, &MainWindow::slotOpenFile);

    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(save_as_action);

    menuBar->addMenu(file_menu);

    {
        auto action_menu = new QMenu("Actions");

        auto split = new QAction("Split");
        split->setShortcut(tr("F9"));
        connect(split, &QAction::triggered, this, &MainWindow::slotSplitAction);

        action_menu->addAction(split);

        menuBar->addMenu(action_menu);
    }

    this->setMenuBar(menuBar);
}

void MainWindow::createStatusBar() {
    statusBar = new QStatusBar;
    this->setStatusBar(statusBar);
}

void MainWindow::newFile(bool trigger) {
    Q_UNUSED(trigger);
    this->newTab();
}

void MainWindow::tabChanged(int index) {
    Q_UNUSED(index);
    updateTabViewMenu();
}

void MainWindow::slotSaveFile(bool checked) {
    auto tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex()));
    if (!tab) return;

    tab->saveToFile();
}

void MainWindow::slotSaveAsFile(bool checked) {
    auto tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex()));
    if (!tab) return;

    qDebug() << "Unimplemented";
}

void MainWindow::slotOpenFile(bool checked) {
    Q_UNUSED(checked)

    auto tab = newTab();
    tab->loadFromFile();
}

void MainWindow::slotSplitAction(bool checked) {

    auto current_tab = dynamic_cast<Tab*>(tabWidget->widget(tabWidget->currentIndex()));
    if (!current_tab) return;

    current_tab->splitAction();
}