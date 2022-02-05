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
#include <DockAreaWidget.h>
#include <DockManager.h>

using namespace ads;

class Tab;

class MainWindow : public QMainWindow {

    enum SaveFileAnswer {
        Save = 0,
        NoSave,
        Cancel
    };

public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:

    void newFile(bool trigger);

    void slotSaveFile(bool checked);

    void slotSaveAsFile(bool checked);

    void slotOpenFile(bool checked);

    void slotSplitAction(bool checked);

private:

    void createMenuBar();

    void createStatusBar();

private:



    QStatusBar* statusBar = nullptr;
    QMenuBar* menuBar = nullptr;

    CDockManager* manager = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H