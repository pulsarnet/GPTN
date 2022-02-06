//
// Created by Николай Муравьев on 11.12.2021.
//

#include "mainwindow.h"


#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>
#include "main_tree/treeview.h"
#include "main_tree/treemodel.h"
#include "main_tree/treeitem.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {

    createMenuBar();
    createStatusBar();

    CDockManager::setConfigFlag(CDockManager::DragPreviewShowsContentPixmap, false);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);

    manager = new CDockManager(this);
    this->setCentralWidget(manager);


    auto internalDockManager = new CDockManager;
    auto centralWidget = new CDockWidget("Central");
    centralWidget->setWidget(internalDockManager);
    manager->setCentralWidget(centralWidget);

    treeModel = new TreeModel(internalDockManager);

    auto treeView = new TreeView;
    treeView->setModel(treeModel);
    auto treeDocker = new CDockWidget("Tree");
    treeDocker->setWidget(treeView);
    manager->addDockWidgetTab(DockWidgetArea::LeftDockWidgetArea, treeDocker);
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
    //this->newTab();
}

void MainWindow::slotSaveFile(bool checked) {
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Address Book"), "",
                                                    tr("Address Book (*.ptn);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return;
    }

    auto data = treeModel->root()->toVariant();
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    out << data;
}

void MainWindow::slotSaveAsFile(bool checked) {

}

void MainWindow::slotOpenFile(bool checked) {

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Address Book"), "",
                                                    tr("Address Book (*.ptn);;All Files (*)"));

    if (fileName.isEmpty())
        return;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return;
    }

    QVariant data;
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    in >> data;

    dynamic_cast<RootTreeItem*>(treeModel->root())->fromVariant(data);
}

void MainWindow::slotSplitAction(bool checked) {

}