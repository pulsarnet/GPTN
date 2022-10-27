//
// Created by Николай Муравьев on 11.12.2021.
//

#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QJsonDocument>
#include <QTableView>
#include <QMessageBox>
#include <DockAreaWidget.h>
#include <DockAreaTitleBar.h>
#include "windows_types/close_on_inactive.h"
#include "ActionTabWidget/ActionTabWidget.h"
#include "ActionTabWidget/DecomposeModelTab.h"
#include "ActionTabWidget/WrappedLayoutWidget.h"
#include "view/GraphicScene.h"
#include "MainTree/MainTreeModel.h"
#include "MainTree/ProjectTreeItem.h"
#include "MainTree/ModelTreeItem.h"
#include "MainTree/MainTreeView.h"
#include "MainTree/DecomposeTreeItem.h"
#include "MainTree/ReachabilityTreeItem.h"
#include "MainTree/AnalysisTreeItem.h"
#include "WindowWidgets/NewProjectWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_changed(false)
    , m_tabWidget(new ActionTabWidget)
    , m_editMenu(new QMenu(tr("&Edit")))
{
    createMenuBar();
    createStatusBar();

    auto treeWidget = new QDockWidget(tr("Tree"), this);
    treeWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    auto treeModel = new MainTreeModel();
    m_treeView = new MainTreeView(treeModel, this);
    treeWidget->setWidget(m_treeView);
    connect(m_treeView,
            &MainTreeView::elementAction,
            this,
            &MainWindow::treeItemAction);

    connect(m_treeView,
            &QTreeView::customContextMenuRequested,
            this,
            &MainWindow::treeItemContextMenuRequested);

    connect(m_treeView->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &MainWindow::treeViewSelectionChanged);

    addDockWidget(Qt::LeftDockWidgetArea, treeWidget);

    // Tab widget
    connect(m_tabWidget
            , &ActionTabWidget::currentChanged
            , this
            , &MainWindow::tabWidgetCurrentChanged);

    setCentralWidget(m_tabWidget);
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

    return saveFile();
}

bool MainWindow::save() {
    return saveFile();
}

bool MainWindow::saveFile() {

    // Get current tree project item
    // Get filepath and save it
    if (!m_currentProject)
        return false;

    auto path = QString::fromStdString(m_currentProject->folder().string());
    auto json =
            qobject_cast<GraphicScene*>(m_currentProject->modelItem()->netModelingTab()->view()->scene())->json();

    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Petri Net Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(path), file.errorString()));
    }

    file.write(json.toJson());

    return true;

}

bool MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Address Book"), "",
                                                    tr("Address Book (*.json)"));

    if (fileName.isEmpty())
        return false;

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return false;
    }

    auto buffer = file.readAll();
    auto document = buffer.isEmpty() ? QJsonDocument() : QJsonDocument::fromJson(buffer);

    auto projectItem = new ProjectTreeItem(file.filesystemFileName());
    auto modelItem = projectItem->modelItem();
    if (!dynamic_cast<GraphicScene*>(modelItem->netModelingTab()->view()->scene())->fromJson(document)) {
        QMessageBox::information(this, tr("Unable to open file"),
                                 file.errorString());
        return false;
    }

    auto treeModel = qobject_cast<MainTreeModel*>(
            qobject_cast<MainTreeView*>(m_treeView)->model()
            );

    treeModel->addChild(projectItem);


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

    auto new_action = new QAction("&New", this);
    new_action->setShortcut(QKeySequence::New);
    connect(new_action, &QAction::triggered, this, &MainWindow::onNewProject);

    auto open_action = new QAction("&Open");
    open_action->setShortcut(QKeySequence::Open);
    connect(open_action, &QAction::triggered, this, &MainWindow::slotOpenFile);

    auto save_action = new QAction("Save");
    save_action->setShortcut(QKeySequence::Save);
    connect(save_action, &QAction::triggered, this, &MainWindow::slotSaveFile);

    auto save_as_action = new QAction("Save as...");
    save_as_action->setShortcut(QKeySequence::SaveAs);
    connect(save_as_action, &QAction::triggered, this, &MainWindow::slotSaveAsFile);

    auto quit_action = new QAction("&Quit");
    quit_action->setShortcut(QKeySequence::Quit);
    connect(quit_action, &QAction::triggered, this, &MainWindow::slotQuit);

    file_menu->addAction(new_action);
    file_menu->addAction(open_action);
    file_menu->addAction(save_action);
    file_menu->addAction(save_as_action);
    file_menu->addAction(quit_action);

    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_redoAction);

    menuBar->addMenu(file_menu);
    menuBar->addMenu(m_editMenu);
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
}

void MainWindow::slotOpenFile(bool checked) {
    open();
}

void MainWindow::onNewProject(bool checked) {
    Q_UNUSED(checked);

    // Open window of project creation
    auto window = new NewProjectWindow(this);
    connect(window, &NewProjectWindow::createProject, this, &MainWindow::onNewProjectCreate);
    window->show();
}

void MainWindow::onNewProjectCreate(const QDir& dir, const QString& name) {
    auto path = dir.absolutePath() + "/" + name + ".json";
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    QFile file(path);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("Petri Net Editor"),
                             tr("Cannot create file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(path), file.errorString()));

        return;
    }

    auto projectItem = new ProjectTreeItem(std::filesystem::path(path.toStdString()));
    auto treeModel = qobject_cast<MainTreeModel*>(
            qobject_cast<MainTreeView*>(m_treeView)->model()
            );

    treeModel->addChild(projectItem);
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
    } else if (auto model = dynamic_cast<ReachabilityTreeItem*>(treeItem); model) {
        auto tab = model->reachabilityTab();

        int index = -1;
        for (int i = 0; i < m_tabWidget->count(); i++) {
            auto it = m_tabWidget->widget(i);
            if (it == tab) {
                index = i;
                break;
            }
        }

        if (index < 0) {
            QString name = QString("Reachability Tree of %1")
                    .arg(dynamic_cast<ProjectTreeItem*>(model->parentItem()->parentItem())->data(0).toString());

            index = m_tabWidget->insertTab(m_tabWidget->count() - 1,
                                           tab,
                                           name);
            m_tabWidget->setTabIcon(index, model->icon());
        }

        m_tabWidget->setCurrentIndex(index);
    } else if (auto model = dynamic_cast<DecomposeTreeItem*>(treeItem); model) {
        auto tab = model->decomposeModelTab();

        int index = -1;
        for (int i = 0; i < m_tabWidget->count(); i++) {
            auto it = m_tabWidget->widget(i);
            if (it == tab) {
                index = i;
                break;
            }
        }

        if (index < 0) {
            QString name = QString("Decompose of %1")
                    .arg(dynamic_cast<ProjectTreeItem*>(model->parentItem()->parentItem())->data(0).toString());

            index = m_tabWidget->insertTab(m_tabWidget->count() - 1,
                                           tab,
                                           name);
            m_tabWidget->setTabIcon(index, model->icon());
        }

        m_tabWidget->setCurrentIndex(index);
    }
}

void MainWindow::treeItemContextMenuRequested(const QPoint &point) {
    auto index = m_treeView->indexAt(point);
    if (!index.isValid()) {
        return;
    }

    auto item = static_cast<MainTreeItem*>(index.internalPointer());
    auto menu = item->contextMenu();
    if (menu) {
        disconnect(item,
                   &MainTreeItem::onChildAdd,
                   this,
                   &MainWindow::slotNeedUpdateTreeView);

        connect(item,
                &MainTreeItem::onChildAdd,
                this,
                &MainWindow::slotNeedUpdateTreeView);

        menu->exec(m_treeView->viewport()->mapToGlobal(point));
    }
}

void MainWindow::slotNeedUpdateTreeView() {
    auto treeModel = qobject_cast<MainTreeModel*>(m_treeView->model());

    m_treeView->expand(m_treeView->currentIndex());
    treeModel->layoutChanged();
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

void MainWindow::slotQuit(bool checked) {
    Q_UNUSED(checked)
    QApplication::quit();
}

void MainWindow::treeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    Q_UNUSED(selected)
    Q_UNUSED(deselected)
    auto index = m_treeView->currentIndex();
    if (!index.isValid()) {
        // set null
        m_currentProject = nullptr;
        return;
    }

    auto item = static_cast<MainTreeItem*>(index.internalPointer());

    // set current project item
    if (auto project = dynamic_cast<ProjectTreeItem*>(item); project) {
        m_currentProject = project;
    } else if (auto project = dynamic_cast<ProjectTreeItem*>(item->parentItem()); project) {
        m_currentProject = project;
    } else if (auto project = dynamic_cast<ProjectTreeItem*>(item->parentItem()->parentItem()); project) {
        m_currentProject = project;
    }

    qDebug() << "Current project: " << m_currentProject->data(0).toString();
}

void MainWindow::tabWidgetCurrentChanged(int index) {
    // set undo redo scene action for NetModelingTab scene only
    auto tab = m_tabWidget->widget(index);

    // First: delete actions
    m_editMenu->removeAction(m_undoAction);
    m_editMenu->removeAction(m_redoAction);
    delete m_undoAction;
    delete m_redoAction;

    if (auto netModelingTab = dynamic_cast<NetModelingTab*>(tab); netModelingTab) {
        auto scene = dynamic_cast<GraphicScene*>(netModelingTab->view()->scene());
        m_undoAction = scene->undoAction();
        m_redoAction = scene->redoAction();
    } else {
        m_undoAction = new QAction("Undo", this);
        m_undoAction->setShortcut(QKeySequence::Undo);
        m_undoAction->setEnabled(false);

        m_redoAction = new QAction("Redo", this);
        m_redoAction->setShortcut(QKeySequence::Redo);
        m_redoAction->setEnabled(false);
    }

    m_editMenu->addAction(m_undoAction);
    m_editMenu->addAction(m_redoAction);
}
