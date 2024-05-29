#include "MainWindow.h"
#if defined(Q_OS_WIN32) && !defined(Q_OS_CYGWIN)
#include <Windows.h>
#endif

#include <QApplication>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QDockWidget>
#include <QJsonDocument>
#include <QMessageBox>
#include <QProcess>
#include "ActionTabWidget/NetModelingTab.h"
#include "Editor/GraphicsView.h"
#include "ActionTabWidget/DecomposeModelTab.h"
#include "WindowWidgets/NewProjectWindow.h"
#include "Settings/RecentProjects.h"
#include "Core/ApplicationProjectController.h"
#include "ActionTabWidget/ActionTabWidgetController.h"
#include "Editor/GraphicsScene.h"
#include "modules/reachability/ReachabilityWindow.h"
#include "overrides/MatrixWindow.h"
#include "Editor/GraphicsSceneActions.h"
#include <QStandardPaths>
#include <ptn/net.h>
#include <config.h>


/*
 * MainWindow содержит:
 * - Окно вкладок ActionTabWidget
 * - Меню приложения Menu
 */
MainWindow::MainWindow(ApplicationProjectController* controller, QWidget *parent)
    : QMainWindow(parent)
    , m_Controller(controller)
{
    m_ActionTabWidgetController = new ActionTabWidgetController(this);
    m_metadata = new ProjectMetadata();

    // Init modeling tab
    m_netModelingTab = new NetModelingTab(this);
    int idx = m_ActionTabWidgetController->addTab("Model", QIcon(":/images/modeling.svg"), m_netModelingTab);
    m_ActionTabWidgetController->setTabCloseable(idx, false);

    createMenuBar();
    createStatusBar();
    onProjectNameChanged();
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

    return m_Controller->openProject(filename, this);
}

bool MainWindow::saveAs() {
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save Petri network file"),
                                                    QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
                                                    tr("Petri network file (*.json);;All Files (*)"));

    if (filename.isEmpty() || !saveFile(filename))
        return false;

    m_metadata->setFilename(filename);
    RecentProjects::addRecentProject(filename);
    onProjectNameChanged();

    return true;
}

bool MainWindow::save() {
    if (m_metadata->filename().isEmpty())
        return saveAs(); //redirect to save as
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
    setWindowModified(false);
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

    if (!qobject_cast<GraphicsScene*>(m_netModelingTab->view()->scene())->fromJson(document)) {
        QMessageBox::information(this,
                                 QApplication::applicationDisplayName(),
                                 tr("Cannot load Petri Net: invalid structure."));
        return false;
    }

    // Установим metadata
    m_metadata->setFilename(filename);
    onProjectNameChanged();

    RecentProjects::addRecentProject(filename);
    return true;
}

void MainWindow::onProjectNameChanged() {
    this->setWindowTitle(QString("%1[*]").arg(this->m_metadata->projectName()));
}

void MainWindow::onDocumentChanged() {
    qDebug() << "on document changed";
    setWindowModified(true);
}

void MainWindow::onWindowSubMenu() {
    qDebug() << "MainWindow::onWindowSubMenu(" << sender() << ")";
    auto menu = static_cast<QMenu*>(sender());
    menu->clear();
    for (auto& [project, window] : m_Controller->openedProjects()) {
        auto act = menu->addAction(window->metadata()->projectName());
        act->setCheckable(true);
        act->setChecked(m_metadata->filename() == project);
        act->setData(window->winId());
    }
}

void MainWindow::onWindowSubMenuAction(QAction *act) {
    auto winID = act->data().toULongLong();
    if (winID == winId()) {
        return;
    }

    // todo: cross-platform
#if defined (Q_OS_WINDOWS) && !defined(Q_OS_CYGWIN)
    SetActiveWindow((HWND)winID);
#endif
}

void MainWindow::onAbout() {
    QMessageBox::about(this,
                       tr("About %1").arg(qApp->applicationDisplayName()),
                       MainWindow::aboutText());
}

QString MainWindow::aboutText() {
    return QString(
            "<b>%1</b><br>"
            "Version: %2<br>"
            "<p>This is a free software.</p>"
            "<hr>"
            "<p>Credits:</p>"
            "<br> - QT framework: <a href=\"http://qt.io\">Site</a>"
            "<br> - Qt Advanced Docking System: <a href=\"https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System\">Site</a>"
            "<br> - Graphviz: <a href=\"https://graphviz.org/license/\">Site</a>"
    )
    .arg(
         QApplication::applicationDisplayName(),
         VERSION
    );
}

QMessageBox::StandardButton MainWindow::onSaveFileAsk() {
    QString text;

    auto filename = m_metadata->filename();
    if (filename.isEmpty()) {
        text = tr("The document has been modified!");
    } else {
        text = tr("The document has been modified!\n"
               "Save file \"%1\"?").arg(filename);
    }

    return QMessageBox::information(
            this,
            tr("Save"),
            text,
            QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
}

void MainWindow::createMenuBar() {
    m_MenuBar = new QMenuBar;
    m_FileMenu = new QMenu(tr("&File"));
    m_EditMenu = new QMenu(tr("&Edit"));
    m_ToolsMenu = new QMenu(tr("&Tools"));
    m_WindowMenu = new QMenu(tr("&Window"));
    m_HelpMenu = new QMenu(tr("&Help"));

    auto newAction = m_FileMenu->addAction("&New", this, &MainWindow::onNewProject);
    newAction->setToolTip(tr("Create New project"));
    newAction->setShortcut(QKeySequence::New);

    auto openAction = m_FileMenu->addAction("&Open", this, &MainWindow::onOpenFile);
    openAction->setToolTip(tr("Open project"));
    openAction->setShortcut(QKeySequence::Open);

    m_RecentSubmenu = m_FileMenu->addMenu(tr("Recent projects"));
    connect(m_RecentSubmenu, &QMenu::aboutToShow, this, &MainWindow::onRecentProjects);
    connect(m_RecentSubmenu, &QMenu::triggered, this, &MainWindow::onOpenRecentProject);

    auto saveAction = m_FileMenu->addAction("&Save", this, &MainWindow::onSaveFile);
    saveAction->setToolTip(tr("Save file"));
    saveAction->setShortcut(QKeySequence::Save);

    auto saveAsAction = m_FileMenu->addAction("Save As", this, &MainWindow::onSaveAsFile);
    saveAsAction->setToolTip(tr("Save project to new location"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);

    auto closeAction = m_FileMenu->addAction("&Close", this, &MainWindow::onClose);
    closeAction->setToolTip(tr("Close current project"));
    closeAction->setShortcut(QKeySequence::Close);

    // load menu edit actions
    auto scene = qobject_cast<GraphicsScene*>(m_netModelingTab->view()->scene());
    auto actions = scene->actions();
    m_EditMenu->addAction(actions->undoAction());
    m_EditMenu->addAction(actions->redoAction());

    m_ToolsMenu->addAction(actions->vAlignmentAction());
    m_ToolsMenu->addAction(actions->hAlignmentAction());
    m_ToolsMenu->addSeparator();
    m_ToolsMenu->addAction(tr("Generate Reachability Tree"), this, &MainWindow::onReachabilityTree);
    m_ToolsMenu->addAction(tr("I/O Matrix view"), this, &MainWindow::onMatrixWindow);
    m_ToolsMenu->addAction(tr("Synthesis"), this, &MainWindow::onSynthesis);

    connect(m_WindowMenu, &QMenu::aboutToShow, this, &MainWindow::onWindowSubMenu);
    connect(m_WindowMenu, &QMenu::triggered, this, &MainWindow::onWindowSubMenuAction);

    auto aboutQt = m_HelpMenu->addAction(tr("About Qt"), this, &QApplication::aboutQt);
    aboutQt->setToolTip(tr("Show dialog about qt"));

    auto about = m_HelpMenu->addAction(tr("About"), this, &MainWindow::onAbout);
    about->setToolTip(tr("Show about dialog"));

    m_MenuBar->addMenu(m_FileMenu);
    m_MenuBar->addMenu(m_EditMenu);
    m_MenuBar->addMenu(m_ToolsMenu);
    m_MenuBar->addMenu(m_WindowMenu);
    m_MenuBar->addMenu(m_HelpMenu);

    this->setMenuBar(m_MenuBar);
}

void MainWindow::createStatusBar() {
    m_statusBar = new QStatusBar;
    this->setStatusBar(m_statusBar);
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
    m_RecentSubmenu->clear();

    auto recentFiles = RecentProjects::getRecentProjects();
    for (auto& file : recentFiles) {
        auto action = new QAction(file, this);
        action->setData(file);
        m_RecentSubmenu->addAction(action);
    }
}

void MainWindow::onOpenRecentProject(QAction* action) {
    Q_ASSERT(action);
    QString path(action->data().toString());
    if (!m_Controller->openProject(path, this)) {
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

    if (!m_Controller->openProject(path, this)) {
        qDebug() << "Unable to create project";
        return;
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    qDebug() << "MainWindow::closeEvent(" << event << ")";
    if (saveOnExit()) {
        if (m_metadata) {
            m_Controller->closeProject(m_metadata->filename());
        }
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::saveOnExit() {
    qDebug() << "MainWindow::saveOnExit()";

    // Check if project is changed
    if (!isWindowModified())
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

void MainWindow::onClose(bool checked) {
    Q_UNUSED(checked)
    this->close();
}

void MainWindow::onReachabilityTree(bool checked) {
    Q_UNUSED(checked)
    Q_ASSERT(m_metadata);
    if (!m_reachabilityTab) {
        // Если еще не был создан модуль Reachability
        auto net = m_metadata->net();
        m_reachabilityTab = new ReachabilityWindow(net);
        m_ActionTabWidgetController->addTab(tr("Reachability Tree"), QIcon(":/images/tree.svg"), m_reachabilityTab);
    } else if (int index = m_ActionTabWidgetController->indexOf(m_reachabilityTab); index >= 0) {
        // Модуль уже открыт
        m_ActionTabWidgetController->setCurrentIndex(index);
    } else {
        // Модуль закрыт, но существует, добавим
        m_ActionTabWidgetController->addTab(tr("Reachability Tree"), QIcon(":/images/tree.svg"), m_reachabilityTab);
    }
    m_reachabilityTab->reload();
}

void MainWindow::onMatrixWindow(bool checked) {
    Q_UNUSED(checked)
    Q_ASSERT(m_metadata);

    if (m_IOMatrixWindow) {
        m_IOMatrixWindow->activateWindow();
    } else {
        auto matrix = m_metadata->net()->as_matrix();
        m_IOMatrixWindow = new MatrixWindow(std::move(std::get<0>(matrix)), std::move(std::get<1>(matrix)), this);
        connect(m_IOMatrixWindow, &MatrixWindow::onWindowClose, this, &MainWindow::onMatrixWindowClose);
        m_IOMatrixWindow->show();
    }
}

void MainWindow::onMatrixWindowClose(QWidget* window) {
    Q_UNUSED(window);
    m_IOMatrixWindow = nullptr;
}

void MainWindow::onSynthesis() {
    if (!m_decomposeTab) {
        m_decomposeTab = new DecomposeModelTab(m_metadata);
        m_ActionTabWidgetController->addTab(tr("Decompose and Synthesis"), QIcon(":/images/decompose.svg"), m_decomposeTab);
    }
}

void MainWindow::onTabChanged(int index) {
    // clear edit menu
    if (index == -1) {

    }
}
