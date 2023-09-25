#ifndef FFI_RUST_MAINWINDOW_H
#define FFI_RUST_MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include <QListView>
#include <QAction>
#include <QToolBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QWheelEvent>
#include <QActionGroup>
#include <QTabWidget>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QMessageBox>
#include <DockManager.h>
#include <QDir>
#include "core/ProjectMetadata.h"

class ActionTabWidget;
class MainTreeItem;
class ApplicationProjectController;
class ActionTabWidgetController;
class MatrixWindow;
class NetModelingTab;
class ReachabilityWindow;

class MainWindow : public QMainWindow {

public:
    MainWindow() = delete;
    MainWindow(const MainWindow&) = delete;
    MainWindow(MainWindow&&) = delete;
    explicit MainWindow(ApplicationProjectController* controller, QWidget *parent = nullptr);

    bool saveAs();
    bool save();

    bool open();
    bool initProject(const QString &filename);

    bool saveFile(const QString& filename);
    bool saveFileInner(QFile& file);
    QMessageBox::StandardButton onSaveFileAsk();

    ProjectMetadata* metadata() const { return m_metadata; }

    void onProjectNameChanged();

public slots:
    void onSaveFile(bool checked);
    void onSaveAsFile(bool checked);
    void onOpenFile(bool checked);
    void onRecentProjects();
    void onOpenRecentProject(QAction* action);
    void onNewProject(bool checked);
    void onNewProjectCreate(const QDir& dir, const QString& name);
    void onClose(bool checked);
    void onDocumentChanged();

    void onWindowSubMenu();
    void onWindowSubMenuAction(QAction*);
    void onAbout();
    static QString aboutText();

    // Net
    void onReachabilityTree(bool checked);

    void onMatrixWindow(bool checked);
    void onMatrixWindowClose(QWidget* window);

    // Tab Changed
    void onTabChanged(int);

protected:

    void closeEvent(QCloseEvent* event) override;

    bool saveOnExit();

private:

    void createMenuBar();

    void createStatusBar();

private:

    ApplicationProjectController* m_Controller;
    ActionTabWidgetController* m_ActionTabWidgetController;

    QStatusBar* m_statusBar = nullptr;

    // Menu bar
    QMenuBar* m_MenuBar = nullptr;
    QMenu* m_FileMenu = nullptr;
    QMenu* m_RecentSubmenu = nullptr;
    QMenu* m_EditMenu = nullptr;
    QMenu* m_ToolsMenu = nullptr;
    QMenu* m_WindowMenu = nullptr;
    QMenu* m_HelpMenu = nullptr;

    ProjectMetadata* m_metadata = nullptr;

    NetModelingTab* m_netModelingTab = nullptr;
    ReachabilityWindow* m_reachabilityTab = nullptr;

    MatrixWindow* m_IOMatrixWindow = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H