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
#include "graphics_view.h"
#include "tab.h"

extern "C" struct PetriNet;

extern "C" PetriNet* make();
extern "C" void del(PetriNet*);
extern "C" unsigned long count(PetriNet*);

class MainWindow : public QMainWindow {

    enum SaveFileAnswer {
        Save = 0,
        NoSave,
        Cancel
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:

    void closeTab(int index);

    void positionChecked(bool checked);

    void transitionChecked(bool checked);

    void moveChecked(bool checked);

    void connectChecked(bool checked);

    void rotateChecked(bool checked);

    void newFile(bool trigger);

    void tabChanged(int index);

    void slotSaveFile(bool checked);

    void slotSaveAsFile(bool checked);

    void slotOpenFile(bool checked);

private:

    static SaveFileAnswer askSaveFile();

    QString chooseSaveFile();

    void configureTab();

    Tab* newTab();

    QAction* makeAction(const QString& name, const QIcon& icon, bool checkable, QActionGroup* actionGroup = nullptr);

    void createMenuBar();

    void createStatusBar();

    void createToolBar();

    GraphicsView* currentScene();

private:

    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;
    QAction* connect_action = nullptr;
    QAction* rotation_action = nullptr;
    QActionGroup* actionGroup = nullptr;

    QToolBar* toolBar = nullptr;

    QStatusBar* statusBar = nullptr;
    QMenuBar* menuBar = nullptr;

    QTabWidget* tabWidget = nullptr;

    PetriNet* net = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H