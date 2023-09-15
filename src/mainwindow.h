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
class ProjectTreeItem;

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
    explicit MainWindow(QWidget *parent = nullptr);

    bool saveAs();
    bool save();

    bool open();
    bool openFile(const QString& fileName);
    bool initProject(const QString &fileName, QJsonDocument&& document);

    bool saveFile();
    QMessageBox::StandardButton onSaveFileAsk();

public slots:

    void slotSaveFile(bool checked);

    void slotSaveAsFile(bool checked);

    void slotOpenFile(bool checked);

    void slotRecentFiles();
    void slotOpenRecentFile();

    void onNewProject(bool checked);
    void onNewProjectCreate(const QDir& dir, const QString& name);

    void treeItemAction(const QModelIndex& index);
    void closeProjectRequested(bool checked);

    void treeItemContextMenuRequested(const QPoint& point);

    void slotNeedUpdateTreeView();

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

    QStatusBar* statusBar = nullptr;
    QMenuBar* menuBar = nullptr;
    ActionTabWidget* m_tabWidget = nullptr;

    QDockWidget* m_treeWidget = nullptr;
    MainTreeView* m_treeView = nullptr;


    QMenu* m_recent_submenu = nullptr;
    QMenu* m_editMenu = nullptr;
    QAction* m_redoAction = nullptr;
    QAction* m_undoAction = nullptr;

    std::vector<QString> m_openedProjects;

    ProjectMetadata* m_metadata = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H