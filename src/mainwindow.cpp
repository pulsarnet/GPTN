//
// Created by Николай Муравьев on 11.12.2021.
//

#include "mainwindow.h"

#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QJsonDocument>
#include <QTableView>
#include <QMessageBox>
#include <QTreeView>
#include <DockAreaWidget.h>
#include <DockAreaTitleBar.h>
#include <DockWidget.h>
#include "windows_types/close_on_inactive.h"
#include "ActionTabWidget/ActionTabWidget.h"
#include "ActionTabWidget/NetModelingTab.h"
#include "view/graphic_scene.h"
#include "MainTree/MainTreeModel.h"
#include "MainTree/ProjectTreeItem.h"
#include "MainTree/ModelTreeItem.h"
#include "MainTree/MainTreeView.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_changed(false), m_tabWidget(new ActionTabWidget) {
    createMenuBar();
    createStatusBar();

    m_dockManager = new ads::CDockManager(this);

    auto treeModel = new MainTreeModel();

#ifdef _WIN32
    treeModel->addChild(new ProjectTreeItem(std::filesystem::path("G:/tmp")));
#else
    treeModel->addChild(new ProjectTreeItem(std::filesystem::path("/tmp")));
#endif

    auto treeView = new MainTreeView(treeModel, this);
    connect(treeView,
            &MainTreeView::elementAction,
            this,
            &MainWindow::treeItemAction);

    m_mainTreeView = new ads::CDockWidget("Tree");
    m_mainTreeView->setWidget(treeView);
    m_mainTreeView->setFeature(ads::CDockWidget::DockWidgetClosable, false);

    m_dockTabWidget = new ads::CDockWidget("Tab widget");
    m_dockTabWidget->setWidget(m_tabWidget);

    auto area = m_dockManager->setCentralWidget(m_dockTabWidget);
    area->titleBar()->hide();

    m_dockManager->addDockWidget(ads::LeftDockWidgetArea, m_mainTreeView);

    setCentralWidget(m_dockManager);
    //setCentralWidget(m_tabWidget);
}

void MainWindow::setFileName(const QString &name) {
    m_filename = name;

    if (m_filename.isEmpty())
        setWindowTitle("Petri Net Editor");
    else
        setWindowTitle("Petri Net Editor - " + m_filename);
}

bool MainWindow::saveAs() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Petri network file"),
                                                    tr("Petri network file (*.json);;All Files (*)"));

    if (filename.isEmpty())
        return false;

    return saveFile(filename);
}

bool MainWindow::save() {
    if (m_filename.isEmpty())
        return saveAs();

    return saveFile(m_filename);
}

bool MainWindow::saveFile(const QString &filename) {

    QFile file(filename);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::critical(this, tr("Unable to save file"), file.errorString());
        return false;
    }

    auto data = dynamic_cast<GraphicScene*>(m_tabWidget->mainTab()->view()->scene())->json();
    auto array = data.toJson();
    file.write(array);

    setFileName(filename);
    m_changed = false;

    return true;

}

bool MainWindow::open() {

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Address Book"), "",
                                                    tr("Address Book (*.json)"));

    if (fileName.isEmpty())
        return false;

    if (m_changed) {
        auto ret = onSaveFileAsk();
        if (ret == QMessageBox::Cancel || (ret == QMessageBox::Save && !save()))
            return false;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return false;
    }

    auto document = QJsonDocument::fromJson(file.readAll());
    if (!dynamic_cast<GraphicScene*>(m_tabWidget->mainTab()->view()->scene())->fromJson(document)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return false;
    }

    setFileName(fileName);
    m_changed = false;

    return true;
}

QMessageBox::StandardButton MainWindow::onSaveFileAsk() {
    return QMessageBox::warning(
            this,
            m_filename,
            tr("The document has been modified.\n"
               "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
}

void MainWindow::createMenuBar() {
    menuBar = new QMenuBar;

    auto file_menu = new QMenu("&File");
    auto save_action = new QAction("Save");
    save_action->setShortcut(tr("Ctrl+S"));
    connect(save_action, &QAction::triggered, this, &MainWindow::slotSaveFile);

    auto save_as_action = new QAction("Save as...");
    save_as_action->setShortcut(tr("Ctrl+Shift+S"));
    connect(save_as_action, &QAction::triggered, this, &MainWindow::slotSaveAsFile);

    auto open_action = new QAction("&Open");
    open_action->setShortcut(tr("Ctrl+O"));
    connect(open_action, &QAction::triggered, this, &MainWindow::slotOpenFile);

    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(save_as_action);

    menuBar->addMenu(file_menu);
    menuBar->addMenu(new QMenu("&Edit"));
    menuBar->addMenu(new QMenu("&Tools"));
    menuBar->addMenu(new QMenu("&Window"));
    menuBar->addMenu(new QMenu("&Help"));

    this->setMenuBar(menuBar);
}

void MainWindow::createStatusBar() {
    statusBar = new QStatusBar;
    this->setStatusBar(statusBar);
}

void MainWindow::slotSaveFile(bool checked) {
    save();
}

void MainWindow::slotSaveAsFile(bool checked) {
    auto window = new CloseOnInActive;
    window->show();
    //saveAs();
}

void MainWindow::slotOpenFile(bool checked) {
    open();
}

void MainWindow::onDocumentChanged() {
    m_changed = true;
}

void MainWindow::treeItemAction(const QModelIndex& index) {
    auto treeItem = static_cast<MainTreeItem*>(index.internalPointer());
    if (auto model = dynamic_cast<ModelTreeItem*>(treeItem); model) {
        auto tab = model->netModelingTab();

        int index = -1;
        for (int i = 0; i < m_tabWidget->count(); i++) {
            auto it = m_tabWidget->widget(i);
            if (it == tab) {
                index = i;
                break;
            }
        }

        if (index < 0) {
            index = m_tabWidget->insertTab(m_tabWidget->count() - 1,
                                   tab,
                                   dynamic_cast<ProjectTreeItem*>(model->parentItem())->data(0).toString());
            m_tabWidget->setTabIcon(index, model->icon());
        }

        m_tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {

    if (saveOnExit()) {
        event->accept();
    }
    else {
        event->ignore();
    }

}

bool MainWindow::saveOnExit() {
    if (!m_changed)
        return true;

    switch (onSaveFileAsk())
    {
        case QMessageBox::Save:
            return save();
        case QMessageBox::Cancel:
            return false;
        default:
            break;
    }

    return true;
}