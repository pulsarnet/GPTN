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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), m_changed(false) {

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

    connect(treeModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onDocumentChanged);
    connect(treeModel, &QAbstractItemModel::rowsInserted, this, &MainWindow::onDocumentChanged);

}

void MainWindow::setFileName(const QString &name) {
    m_filename = name;

    if (m_filename.isEmpty())
        setWindowTitle("Petri Net Editor");
    else
        setWindowTitle("Petri Net Editor - " + m_filename);
}

bool MainWindow::saveAs() {

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save Address Book"), "",
                                                    tr("Address Book (*.ptn);;All Files (*)"));

    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool MainWindow::save() {
    if (m_filename.isEmpty())
        return saveAs();

    return saveFile(m_filename);
}

bool MainWindow::saveFile(const QString &filename) {

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return false;
    }

    auto data = treeModel->root()->toVariant();
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_6_0);
    out << data;

    setFileName(filename);
    m_changed = false;

    return true;

}

bool MainWindow::open() {

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Address Book"), "",
                                                    tr("Address Book (*.ptn);;All Files (*)"));

    if (fileName.isEmpty())
        return false;

    if (m_changed) {
        auto ret = onSaveFileAsk();
        if (ret == QMessageBox::Cancel || (ret == QMessageBox::Save && !save()))
            return false;
    }

    treeModel->removeRows(0, treeModel->root()->childCount(), treeModel->indexForTreeItem(treeModel->root()));
    //treeModel->root()->removeChildren(0, treeModel->root()->childCount());

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return false;
    }

    QVariant data;
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_6_0);
    in >> data;

    dynamic_cast<RootTreeItem*>(treeModel->root())->fromVariant(data);

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

    auto open_action = new QAction("Open");
    open_action->setShortcut(tr("Ctrl+O"));
    connect(open_action, &QAction::triggered, this, &MainWindow::slotOpenFile);

    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(save_as_action);

    menuBar->addMenu(file_menu);

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
    saveAs();
}

void MainWindow::slotOpenFile(bool checked) {
    open();
}

void MainWindow::onDocumentChanged() {
    m_changed = true;
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