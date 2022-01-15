//
// Created by Николай Муравьев on 11.12.2021.
//

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
#include "view/graphics_view.h"
#include "tab.h"
#include "petri_net.h"
#include "toolbox/toolbox.h"
#include "view/graphics_scene.h"

#include <DockAreaWidget.h>
#include <DockManager.h>

using namespace ads;


class MainWindow : public QMainWindow {

    enum SaveFileAnswer {
        Save = 0,
        NoSave,
        Cancel
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void addTabFromNet(InnerCommonResult common_result, Tab* current);

public slots:

    void closeTab(int index);

    void positionChecked(bool checked);

    void transitionChecked(bool checked);

    void moveChecked(bool checked);

    void connectChecked(bool checked);

    void rotateChecked(bool checked);

    void removeChecked(bool checked);

    void markerChecked(bool checked);

    void newFile(bool trigger);

    void tabChanged(int index);

    void slotSaveFile(bool checked);

    void slotSaveAsFile(bool checked);

    void slotOpenFile(bool checked);

    void slotSplitAction(bool checked);

    void slotSplitChecked(const QModelIndex& index);

private:

    static SaveFileAnswer askSaveFile();

    QString chooseSaveFile();

    void configureTab();

    Tab* newTab();

    QAction* makeAction(const QString& name, const QIcon& icon, bool checkable, QActionGroup* actionGroup = nullptr);

    void createMenuBar();

    void createStatusBar();

    void createToolBar();

    void updateTabViewMenu() {
        if (auto tab = tabWidget->currentWidget(); tab) {
            auto menu = dynamic_cast<Tab*>(tab)->menuOfDockToggle();
            dynamic_cast<QToolButton*>(tabWidget->cornerWidget())->setMenu(menu);
        }
    }

    GraphicScene* currentScene() {
        return qobject_cast<Tab*>(tabWidget->currentWidget())->scene();
    }

private:

    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;
    QAction* connect_action = nullptr;
    QAction* rotation_action = nullptr;
    QAction* remove_action = nullptr;
    QAction* marker_action = nullptr;
    QActionGroup* actionGroup = nullptr;

    ToolBox* toolBar = nullptr;

    QStatusBar* statusBar = nullptr;
    QMenuBar* menuBar = nullptr;

    QTabWidget* tabWidget = nullptr;

    CDockManager* manager = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H