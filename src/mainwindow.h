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

class QTreeView;
class ActionTabWidget;
class MainTreeItem;
class MainTreeView;
class ApplicationProjectController;
class MainTreeController;
class ActionTabWidgetController;

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

    bool saveFile();
    QMessageBox::StandardButton onSaveFileAsk();

public slots:

    void onSaveFile(bool checked);

    void onSaveAsFile(bool checked);

    void onOpenFile(bool checked);

    void onRecentProjects();
    void onOpenRecentProject();

    void onNewProject(bool checked);
    void onNewProjectCreate(const QDir& dir, const QString& name);

    void treeItemAction(const QModelIndex& index);

    void treeItemContextMenuRequested(const QPoint& point);

    void slotQuit(bool checked);

    void treeViewSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

    void tabWidgetCurrentChanged(int index);

protected:

    void closeEvent(QCloseEvent* event) override;

    bool saveOnExit();

private:

    void createMenuBar();

    void createStatusBar();

private:

    ApplicationProjectController* mController;
    MainTreeController* mTreeController;
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
};

#endif //FFI_RUST_MAINWINDOW_H