#include "MainWindow.h"

#include <QStyle>
#include <QApplication>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QJsonDocument>
#include <QMessageBox>
#include <QProcess>
#include "ActionTabWidget/ActionTabWidget.h"
#include "ActionTabWidget/DecomposeModelTab.h"
#include "WindowWidgets/NewProjectWindow.h"
#include "Settings/RecentProjects.h"
#include "Core/ApplicationProjectController.h"
#include "ActionTabWidget/ActionTabWidgetController.h"
#include "Editor/GraphicsScene.h"
#include "Core/FFI/rust.h"
#include "modules/reachability/reachability_window.h"
#include "ActionTabWidget/WrappedLayoutWidget.h"
#include "overrides/MatrixWindow.h"
#include "Editor/GraphicsSceneActions.h"
#include <QStandardPaths>

/*
 * MainWindow содержит:
 * - Окно вкладок ActionTabWidget
 * - Меню приложения Menu
 */
MainWindow::MainWindow(ApplicationProjectController* controller, QWidget *parent)
    : QMainWindow(parent)
    , mController(controller)
{
    mActionTabWidgetController = new ActionTabWidgetController(this);

    createMenuBar();
    createStatusBar();
}

/**
 * Выбрать и открыть файл.
 *
 * @return true - если открыт новый проект, false - если не удалось открыть
 */
bool MainWindow::open() {
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Petri Net"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("JSON Petri Net (*.json)"));
    if (filename.isEmpty())
        return false;

    return mController->openProject(filename, this);
}

bool MainWindow::saveAs() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Petri network file"),
                                                    tr("Petri network file (*.json);;All Files (*)"));

    if (filename.isEmpty())
        return false;

    if (saveFile(filename)) {
        m_metadata->setFilename(filename);
        return true;
    }
    return false;
}

bool MainWindow::save() {
    return saveFile(m_metadata->filename());
}

bool MainWindow::saveFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        QMessageBox::information(this,
                                 QApplication::applicationDisplayName(),
                                 tr("Unable to open file").arg(file.errorString()));
        return false;
    }

    saveFileInner(file);
    m_metadata->setChanged(false);
    return true;
}

bool MainWindow::saveFileInner(QFile& file) {
    Q_ASSERT(file.isOpen());

    auto json = qobject_cast<GraphicsScene*>(m_netModelingTab->view()->scene())->json();
    QTextStream out(&file);
    out << json.toJson(QJsonDocument::Compact);
    out.flush();
    return true;
}

/**
 * Инициализирует проект в текущем окне
 */
bool MainWindow::initProject(const QString &filename) {
    // Open JSON
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this,
                                 QApplication::applicationDisplayName(),
                                 tr("Unable to open file").arg(file.errorString()));
        return false;
    }

    QByteArray objectData = file.readAll();
    QJsonParseError error;
    QJsonDocument document;
    if (!objectData.isEmpty()) {
        document = QJsonDocument::fromJson(objectData, &error);
        if (error.error != QJsonParseError::NoError) {
            QMessageBox::information(this,
                                     QApplication::applicationDisplayName(),
                                     tr("Invalid file data: %1").arg(error.errorString()));
            return false;
        }
    }

    // Установим metadata
    m_metadata = new ProjectMetadata(filename);
    m_netModelingTab = new NetModelingTab(this);
    auto scene = qobject_cast<GraphicsScene*>(m_netModelingTab->view()->scene());
    if (!scene->fromJson(document)) {
        delete m_netModelingTab;
        m_netModelingTab = nullptr;
        return false;
    }

    connect(scene, &GraphicsScene::sceneChanged, this, &MainWindow::onDocumentChanged);

    int idx = mActionTabWidgetController->addTab("Model", QIcon(":/images/modeling.svg"), m_netModelingTab);
    mActionTabWidgetController->setTabCloseable(idx, false);

    this->setWindowTitle(this->m_metadata->projectName());

    // load menu edit actions
    auto actions = scene->actions();
    mEditMenu->addAction(actions->undoAction());
    mEditMenu->addAction(actions->redoAction());

    RecentProjects::addRecentProject(filename);
    return true;
}

void MainWindow::onDocumentChanged() {
    qDebug() << "on document changed";
    m_metadata->setChanged(true);
}

QMessageBox::StandardButton MainWindow::onSaveFileAsk() {
    return QMessageBox::warning(
            this,
            m_metadata->filename(),
            tr("The document has been modified.\n"
               "Do you want to save your changes?"),
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
}

void MainWindow::createMenuBar() {
    mMenuBar = new QMenuBar;
    mFileMenu = new QMenu(tr("&File"));
    mEditMenu = new QMenu(tr("&Edit"));
    mViewMenu = new QMenu(tr("&View"));
    mToolsMenu = new QMenu(tr("&Tools"));
    mWindowMenu = new QMenu(tr("&Window"));
    mHelpMenu = new QMenu(tr("&Help"));

    auto open_action = new QAction("&Open");
    open_action->setShortcut(QKeySequence::Open);
    connect(open_action, &QAction::triggered, this, &MainWindow::onOpenFile);

    auto save_action = new QAction("Save");
    save_action->setShortcut(QKeySequence::Save);
    connect(save_action, &QAction::triggered, this, &MainWindow::onSaveFile);

    auto save_as_action = new QAction("Save as...");
    save_as_action->setShortcut(QKeySequence::SaveAs);
    connect(save_as_action, &QAction::triggered, this, &MainWindow::onSaveAsFile);

    auto quit_action = new QAction("&Quit");
    quit_action->setShortcut(QKeySequence::Quit);
    connect(quit_action, &QAction::triggered, this, &MainWindow::onQuit);

    auto newAction = mFileMenu->addAction( "&New", this, &MainWindow::onNewProject);
    newAction->setToolTip(tr("Create New project"));
    newAction->setShortcut(QKeySequence::New);

    auto openAction = mFileMenu->addAction("&Open", this, &MainWindow::onOpenFile);
    openAction->setToolTip(tr("Open project"));
    openAction->setShortcut(QKeySequence::Open);

    mRecentSubmenu = mFileMenu->addMenu(tr("Recent projects"));
    connect(mRecentSubmenu, &QMenu::aboutToShow, this, &MainWindow::onRecentProjects);

    auto saveAction = mFileMenu->addAction("&Save", this, &MainWindow::onSaveFile);
    saveAction->setToolTip(tr("Save file"));
    saveAction->setShortcut(QKeySequence::Save);

    auto saveAsAction = mFileMenu->addAction("Save As", this, &MainWindow::onSaveAsFile);
    saveAsAction->setToolTip(tr("Save project to new location"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    auto closeAction = mFileMenu->addAction("&Close", this, &MainWindow::onSaveAsFile);
    closeAction->setToolTip(tr("Close current project"));
    closeAction->setShortcut(QKeySequence::Close);

    // on tab changed
    //auto sceneActions = qobject_cast<GraphicsScene*>(m_netModelingTab->view()->scene())->actions();
    //mEditMenu->addAction(sceneActions->undoAction());
    //mEditMenu->addAction(sceneActions->redoAction());

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
    if (!m_metadata) return;
    save();
}

void MainWindow::onSaveAsFile(bool checked) {
    Q_UNUSED(checked)
    if (!m_metadata) return;
    saveAs();
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
    if (!action)
        return;

    QString path(action->data().toString());
    if (!mController->openProject(path, this)) {
        RecentProjects::removeRecentProject(path);
        return;
    }
}

void MainWindow::onNewProject(bool checked) {
    Q_UNUSED(checked);

    // Open window of project creation
    auto window = new NewProjectWindow(this);
    connect(window, &NewProjectWindow::createProject, this, &MainWindow::onNewProjectCreate);
    window->show();
}

void MainWindow::onNewProjectCreate(const QDir& dir, const QString& name) {
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    QDir finalDir = dir.filePath(name + ".json");
    QString path = finalDir.path();
    // try create
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to create project";
        return;
    }
    file.close();

    if (!mController->openProject(path, this)) {
        qDebug() << "Unable to create project";
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "MainWindow::closeEvent(" << event << ")";
    if (saveOnExit()) {
        if (m_metadata) {
            mController->closeProject(m_metadata->filename());
        }
        event->accept();
    } else {
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

void MainWindow::onQuit(bool checked) {
    Q_UNUSED(checked)
    QApplication::quit();
}

void MainWindow::onReachabilityTree(bool checked) {
    Q_UNUSED(checked)
    Q_ASSERT(m_metadata);
    if (!m_reachabilityTab) {
        // Если еще не был создан модуль Reachability
        auto net = m_metadata->context()->net();
        m_reachabilityTab = new ReachabilityWindow(net);
        mActionTabWidgetController->addTab("Reachability Tree", QIcon(":/images/tree.svg"), m_reachabilityTab);
    } else if (int index = mActionTabWidgetController->indexOf(m_reachabilityTab); index >= 0) {
        // Модуль уже открыт
        mActionTabWidgetController->setCurrentIndex(index);
    } else {
        // Модуль закрыт, но существует, добавим
        mActionTabWidgetController->addTab("Reachability Tree", QIcon(":/images/tree.svg"), m_reachabilityTab);
    }
    m_reachabilityTab->reload();
}

void MainWindow::onMatrixWindow(bool checked) {
    Q_UNUSED(checked)
    Q_ASSERT(m_metadata);

    if (m_IOMatrixWindow) {
        m_IOMatrixWindow->activateWindow();
    } else {
        auto matrix = m_metadata->context()->net()->as_matrix();
        m_IOMatrixWindow = new MatrixWindow(matrix.first, matrix.second, this);
        connect(m_IOMatrixWindow, &MatrixWindow::onWindowClose, this, &MainWindow::onMatrixWindowClose);
        m_IOMatrixWindow->show();
    }
}

void MainWindow::onMatrixWindowClose(QWidget* window) {
    Q_UNUSED(window);
    m_IOMatrixWindow = nullptr;
}

void MainWindow::onTabChanged(int index) {
    // clear edit menu

    if (index == -1)
        return;


}