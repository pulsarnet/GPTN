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

    enum SaveFileAnswer {
        Save = 0,
        NoSave,
        Cancel
    };

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

public slots:
    void onSaveFile(bool checked);
    void onSaveAsFile(bool checked);
    void onOpenFile(bool checked);
    void onRecentProjects();
    void onOpenRecentProject();
    void onNewProject(bool checked);
    void onNewProjectCreate(const QDir& dir, const QString& name);
    void onClose(bool checked);
    void onDocumentChanged();

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

    ApplicationProjectController* mController;
    ActionTabWidgetController* mActionTabWidgetController;

    QStatusBar* statusBar = nullptr;

    // Menu bar
    QMenuBar* mMenuBar = nullptr;
    QMenu* mFileMenu = nullptr;
    QMenu* mRecentSubmenu = nullptr;
    QMenu* mEditMenu = nullptr;
    QMenu* mViewMenu = nullptr;
    QMenu* mToolsMenu = nullptr;
    QMenu* mWindowMenu = nullptr;
    QMenu* mHelpMenu = nullptr;

    ProjectMetadata* m_metadata = nullptr;

    NetModelingTab* m_netModelingTab = nullptr;
    ReachabilityWindow* m_reachabilityTab = nullptr;

    MatrixWindow* m_IOMatrixWindow = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H