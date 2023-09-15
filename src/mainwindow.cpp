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
#include <QProcess>
#include "windows_types/close_on_inactive.h"
#include "ActionTabWidget/ActionTabWidget.h"
#include "ActionTabWidget/DecomposeModelTab.h"
#include "ActionTabWidget/WrappedLayoutWidget.h"
#include "view/GraphicScene.h"
#include "MainTree/MainTreeModel.h"
#include "MainTree/ModelTreeItem.h"
#include "MainTree/MainTreeView.h"
#include "MainTree/DecomposeTreeItem.h"
#include "MainTree/ReachabilityTreeItem.h"
#include "WindowWidgets/NewProjectWindow.h"
#include "Settings/RecentProjects.h"

/*
 * MainWindow содержит:
 * - Дерево модулей проекта MainTreeView
 * - Окно вкладок ActionTabWidget
 * - Меню приложения Menu
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(new ActionTabWidget)
{
    m_treeWidget = new QDockWidget(tr("Project Tree"), this);
    m_treeWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    auto treeModel = new MainTreeModel();
    m_treeView = new MainTreeView(treeModel, this);
    m_treeWidget->setWidget(m_treeView);
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

    addDockWidget(Qt::LeftDockWidgetArea, m_treeWidget);

    // Tab widget
    connect(m_tabWidget
            , &ActionTabWidget::currentChanged
            , this
            , &MainWindow::tabWidgetCurrentChanged);

    setCentralWidget(m_tabWidget);

    createMenuBar();
    createStatusBar();
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
    if (!m_metadata)
        return false;

    // TODO: Переделать
//    auto path = m_metadata->fileName();
//    auto json =
//            qobject_cast<GraphicScene*>(m_currentProject->modelItem()->netModelingTab()->view()->scene())->json();
//
//    QFile file(path);
//    if (!file.open(QFile::WriteOnly)) {
//        QMessageBox::warning(this, tr("Petri Net Editor"),
//                             tr("Cannot write file %1:\n%2.")
//                             .arg(QDir::toNativeSeparators(path), file.errorString()));
//    }
//
//    file.write(json.toJson());

    return true;

}

/**
 * Выбрать и открыть файл.
 *
 * @return true - если открыт новый проект, false - если не удалось открыть
 */
bool MainWindow::open() {
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Petri Net"), "",
                                                    tr("JSON Petri Net (*.json)"));
    if (fileName.isEmpty())
        return false;

    return openFile(fileName);
}

bool MainWindow::openFile(const QString &fileName) {
    QFile file(fileName);

    // TODO: Найти в открытых проектах

    // Try open project file
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),file.errorString());
        return false;
    }

    // Parse JSON data
    QJsonParseError error;
    auto buffer = file.readAll();
    auto document = buffer.isEmpty() ? QJsonDocument() : QJsonDocument::fromJson(buffer, &error);
    if (error.error != QJsonParseError::NoError) {
        QMessageBox::information(this, tr("Failed parse json data"),error.errorString());
        return false;
    }

    if (m_metadata) {
        // if there is opened project in window
        auto newWindow = new MainWindow(nullptr);
        if (!newWindow->initProject(fileName, std::move(document))) {
            delete newWindow;
            return false;
        }
        newWindow->setMinimumSize(QSize(1280, 720));
        newWindow->show();
    } else {
        if (!initProject(fileName, std::move(document))) {
            return false;
        }
    }

    return true;
}

/**
 * Инициализирует проект в текущем окне
 */
bool MainWindow::initProject(const QString &fileName, QJsonDocument &&document) {
    // Установим metadata
    m_metadata = new ProjectMetadata(fileName);
    auto modelTab = new NetModelingTab(m_metadata);
    auto scene = qobject_cast<GraphicScene*>(modelTab->view()->scene());
    if (!scene->fromJson(document)) {
        delete modelTab;
        return false;
    }

    // Try add to tree
    auto treeItem = new ModelTreeItem(nullptr, modelTab);
    auto treeModel = qobject_cast<MainTreeModel*>(m_treeView->model());
    if (!treeModel->addChild(treeItem)) {
        delete treeItem;
        return false;
    }

    // Установим данные окна (todo: отдельная функция)
    this->setWindowTitle(this->m_metadata->projectName());

    RecentProjects::addRecentProject(fileName);
    return true;
}

QMessageBox::StandardButton MainWindow::onSaveFileAsk() {
    return QMessageBox::warning(
            this,
            m_metadata->fileName(),
            tr("The document has been modified.\n"
               "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
}

void MainWindow::createMenuBar() {
    mMenuBar = new QMenuBar;
    mFileMenu = new QMenu(tr("&File"));
    mRecentSubmenu = new QMenu(tr("Recent projects"));
    mEditMenu = new QMenu(tr("&Edit"));
    mViewMenu = new QMenu(tr("&View"));
    mToolsMenu = new QMenu(tr("&Tools"));
    mWindowMenu = new QMenu(tr("&Window"));
    mHelpMenu = new QMenu(tr("&Help"));

    auto new_action = new QAction("&New", this);
    new_action->setShortcut(QKeySequence::New);
    connect(new_action, &QAction::triggered, this, &MainWindow::onNewProject);

    auto open_action = new QAction("&Open");
    open_action->setShortcut(QKeySequence::Open);
    connect(open_action, &QAction::triggered, this, &MainWindow::onOpenFile);

    connect(mRecentSubmenu, &QMenu::aboutToShow, this, &MainWindow::onRecentProjects);

    auto save_action = new QAction("Save");
    save_action->setShortcut(QKeySequence::Save);
    connect(save_action, &QAction::triggered, this, &MainWindow::onSaveFile);

    auto save_as_action = new QAction("Save as...");
    save_as_action->setShortcut(QKeySequence::SaveAs);
    connect(save_as_action, &QAction::triggered, this, &MainWindow::onSaveAsFile);

    auto quit_action = new QAction("&Quit");
    quit_action->setShortcut(QKeySequence::Quit);
    connect(quit_action, &QAction::triggered, this, &MainWindow::slotQuit);

    mFileMenu->addAction(new_action);
    mFileMenu->addAction(open_action);
    mFileMenu->addMenu(mRecentSubmenu);
    mFileMenu->addAction(save_action);
    mFileMenu->addAction(save_as_action);
    mFileMenu->addAction(quit_action);

    mEditMenu->addAction(mUndoAction);
    mEditMenu->addAction(mRedoAction);

    mViewMenu->addAction(m_treeWidget->toggleViewAction());

    mWindowMenu->addAction(m_treeWidget->toggleViewAction());

    mMenuBar->addMenu(mFileMenu);
    mMenuBar->addMenu(mEditMenu);
    mMenuBar->addMenu(mViewMenu);
    mMenuBar->addMenu(mToolsMenu);
    mMenuBar->addMenu(mWindowMenu);
    mMenuBar->addMenu(mHelpMenu);

    this->setMenuBar(mMenuBar);
}

void MainWindow::createStatusBar() {
    statusBar = new QStatusBar;
    this->setStatusBar(statusBar);
}

void MainWindow::onSaveFile(bool checked) {
    Q_UNUSED(checked)
    save();
}

void MainWindow::onSaveAsFile(bool checked) {
    Q_UNUSED(checked)
    auto window = new CloseOnInActive;
    window->show();
}

void MainWindow::onOpenFile(bool checked) {
    Q_UNUSED(checked)
    open();
}

void MainWindow::onRecentProjects() {
    mRecentSubmenu->clear();

    auto recentFiles = RecentProjects::getRecentProjects();
    for (auto& file : recentFiles) {
        auto action = new QAction(file, this);
        action->setData(file);
        connect(action, &QAction::triggered, this, &MainWindow::onOpenRecentProject);
        mRecentSubmenu->addAction(action);
    }
}

void MainWindow::onOpenRecentProject() {
    auto action = qobject_cast<QAction*>(sender());
    if (action)
        openFile(action->data().toString());
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

    // create
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to create project";
        return;
    }

    if (!openFile(path)) {
        qDebug() << "Unable to create project";
        return;
    }
}

void MainWindow::treeItemAction(const QModelIndex& index) {
    auto treeItem = static_cast<MainTreeItem*>(index.internalPointer());
    if (auto model = dynamic_cast<ModelTreeItem*>(treeItem); model) {
        qDebug() << "Opening ModelTreeItem tab";
        auto tab = model->netModelingTab();

        int idx = -1;
        for (int i = 0; i < m_tabWidget->count(); i++) {
            auto it = m_tabWidget->widget(i);
            if (it == tab) {
                idx = i;
                break;
            }
        }

        if (idx < 0) {
            idx = m_tabWidget->insertTab(
                    m_tabWidget->count() - 1,
                    tab,
                    "Model"
                    //dynamic_cast<ProjectTreeItem*>(model->parentItem())->data(0).toString()
            );
            m_tabWidget->setTabIcon(idx, model->icon());
        }

        m_tabWidget->setCurrentIndex(idx);
    } else if (auto reachability = dynamic_cast<ReachabilityTreeItem*>(treeItem); reachability) {
        auto tab = reachability->reachabilityTab();

        int idx = -1;
        for (int i = 0; i < m_tabWidget->count(); i++) {
            auto it = m_tabWidget->widget(i);
            if (it == tab) {
                idx = i;
                break;
            }
        }

        if (idx < 0) {
            QString name(tr("Reachability Tree"));
            idx = m_tabWidget->insertTab(m_tabWidget->count() - 1,
                                         tab,
                                         name);
            m_tabWidget->setTabIcon(idx, reachability->icon());
        }

        m_tabWidget->setCurrentIndex(idx);
    } else if (auto decompose = dynamic_cast<DecomposeTreeItem*>(treeItem); decompose) {
        auto tab = decompose->decomposeModelTab();

        int idx = -1;
        for (int i = 0; i < m_tabWidget->count(); i++) {
            auto it = m_tabWidget->widget(i);
            if (it == tab) {
                idx = i;
                break;
            }
        }

        if (idx < 0) {
            QString name(tr("Decompose"));
            idx = m_tabWidget->insertTab(m_tabWidget->count() - 1,
                                         tab,
                                         name);
            m_tabWidget->setTabIcon(idx, decompose->icon());
        }

        m_tabWidget->setCurrentIndex(idx);
    }
}

void MainWindow::closeProjectRequested(bool checked) {
    Q_UNUSED(checked)

    // TODO: Новая механика
//    auto current = m_treeView->currentIndex();
//    if (!current.isValid())
//        return;
//
//    qDebug() << "MainWindow::closeProjectRequested(index = " << current << ")";
//
//    auto treeItem = static_cast<MainTreeItem*>(current.internalPointer());
//    auto project = dynamic_cast<ProjectTreeItem*>(treeItem);
//    if (!project)
//        return;
//
//
//    // Удалить вкладки (model, analysis) {
//    auto idx = m_tabWidget->findTabContainsWidget(project->modelItem()->netModelingTab());
//    if (idx >= 0) m_tabWidget->removeTab(idx);
//
//    auto analysis_children = project->analysisItem()->childCount();
//    for (int i = 0; i < analysis_children; i++) {
//        auto tab = project->analysisItem()->childItem(i);
//        if (auto reachability = dynamic_cast<ReachabilityTreeItem*>(tab); reachability) {
//            auto idx = m_tabWidget->findTabContainsWidget(reachability->reachabilityTab());
//            if (idx >= 0) m_tabWidget->removeTab(idx);
//        } else if (auto decompose = dynamic_cast<DecomposeTreeItem*>(tab); decompose) {
//            auto idx = m_tabWidget->findTabContainsWidget(decompose->decomposeModelTab());
//            if (idx >= 0) m_tabWidget->removeTab(idx);
//        }
//    }
//    // } Удалить вкладки (model, analysis)
//
//    // Удалить item из дерева
//    m_treeView->model()->removeRow(current.row(), current.parent());
}

void MainWindow::treeItemContextMenuRequested(const QPoint &point) {
    Q_UNUSED(point)
    // TODO: Новая механика
//    auto index = m_treeView->indexAt(point);
//    if (!index.isValid()) {
//        return;
//    }
//
//    auto item = static_cast<MainTreeItem*>(index.internalPointer());
//    if (auto project = dynamic_cast<ProjectTreeItem*>(item); project) {
//        auto menu = new QMenu;
//        menu->deleteLater();
//
//        auto closeProjectAction = new QAction("Close project", menu);
//        connect(
//                closeProjectAction,
//                &QAction::triggered,
//                this,
//                &MainWindow::closeProjectRequested
//                );
//        menu->addAction(closeProjectAction);
//        menu->exec(m_treeView->viewport()->mapToGlobal(point));
//    } else {
//        auto menu = item->contextMenu();
//        if (menu) {
//            disconnect(item,
//                       &MainTreeItem::onChildAdd,
//                       this,
//                       &MainWindow::slotNeedUpdateTreeView);
//
//            connect(item,
//                    &MainTreeItem::onChildAdd,
//                    this,
//                    &MainWindow::slotNeedUpdateTreeView);
//
//            menu->exec(m_treeView->viewport()->mapToGlobal(point));
//        }
//    }
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
    qDebug() << "MainWindow::saveOnExit()";

    // Check if project is changed
    if (!m_metadata || !m_metadata->isChanged())
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
    // TODO: А может функция уже не нужна
//    auto index = m_treeView->currentIndex();
//    if (!index.isValid()) {
//        // set null
//        return;
//    }
//
//    auto item = static_cast<MainTreeItem*>(index.internalPointer());

    // TODO: REMOVE =====> set current project item
//    if (auto project = dynamic_cast<ProjectTreeItem*>(item); project) {
//        m_currentProject = project;
//    } else if (auto project = dynamic_cast<ProjectTreeItem*>(item->parentItem()); project) {
//        m_currentProject = project;
//    } else if (auto project = dynamic_cast<ProjectTreeItem*>(item->parentItem()->parentItem()); project) {
//        m_currentProject = project;
//    }
    //qDebug() << "Current project: " << m_currentProject->data(0).toString();
}

void MainWindow::tabWidgetCurrentChanged(int index) {
    // set undo redo scene action for NetModelingTab scene only
    auto tab = m_tabWidget->widget(index);

    // First: delete actions
    mEditMenu->removeAction(mUndoAction);
    mEditMenu->removeAction(mRedoAction);
    delete mUndoAction;
    delete mRedoAction;

    if (auto netModelingTab = dynamic_cast<NetModelingTab*>(tab); netModelingTab) {
        auto scene = dynamic_cast<GraphicScene*>(netModelingTab->view()->scene());
        mUndoAction = scene->undoAction();
        mRedoAction = scene->redoAction();
    } else {
        mUndoAction = new QAction("Undo", this);
        mUndoAction->setShortcut(QKeySequence::Undo);
        mUndoAction->setEnabled(false);

        mRedoAction = new QAction("Redo", this);
        mRedoAction->setShortcut(QKeySequence::Redo);
        mRedoAction->setEnabled(false);
    }

    mEditMenu->addAction(mUndoAction);
    mEditMenu->addAction(mRedoAction);
}
