#include "MainWindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QJsonDocument>
#include <QMessageBox>
#include <QProcess>
#include "windows_types/close_on_inactive.h"
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
                                                    "",
                                                    tr("JSON Petri Net (*.json)"));
    if (filename.isEmpty())
        return false;

    return mController->openProject(filename);
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
 * Инициализирует проект в текущем окне
 */
bool MainWindow::initProject(const QString &filename) {
    // Open JSON
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(this, tr("Unable to open file"),file.errorString());
        return false;
    }

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        QMessageBox::information(this, tr("Failed parse json data"),error.errorString());
        return false;
    }

    // Установим metadata
    m_metadata = new ProjectMetadata(filename);
    auto modelTab = new NetModelingTab(this);
    auto scene = qobject_cast<GraphicsScene*>(modelTab->view()->scene());
    if (!scene->fromJson(document)) {
        delete modelTab;
        return false;
    }

    mActionTabWidgetController->openTab("Model", QIcon(":/images/modeling.svg"), modelTab);

    this->setWindowTitle(this->m_metadata->projectName());

    RecentProjects::addRecentProject(filename);
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
    connect(quit_action, &QAction::triggered, this, &MainWindow::onQuit);

    mFileMenu->addAction(new_action);
    mFileMenu->addAction(open_action);
    mFileMenu->addMenu(mRecentSubmenu);
    mFileMenu->addAction(save_action);
    mFileMenu->addAction(save_as_action);
    mFileMenu->addAction(quit_action);

    // todo: СДЕЛАТЬ!!!!
    //auto sceneActions = mTreeController->netModeling
//    mEditMenu->addAction(mUndoAction);
//    mEditMenu->addAction(mRedoAction);

    //mViewMenu->addAction(mTreeController->toggleViewAction());

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
    if (!action)
        return;

    QString path(action->data().toString());
    if (!mController->openProject(path)) {
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
    auto path = dir.absolutePath() + "/" + name + ".json";
    if (!dir.exists())
        dir.mkpath(dir.absolutePath());

    // create
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Unable to create project";
        return;
    }

    if (!mController->openProject(path)) {
        qDebug() << "Unable to create project";
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "MainWindow::closeEvent(" << event << ")";
    if (saveOnExit()) {
        if (m_metadata) {
            mController->closeProject(m_metadata->fileName());
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

    auto net = m_metadata->context()->net();
    auto reachability = net->reachability();
    auto reachabilityWindow = new WrappedLayoutWidget(new ReachabilityWindow(net, reachability));
    mActionTabWidgetController->openTab("Reachability Tree", QIcon(":/images/tree.svg"), reachabilityWindow);
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