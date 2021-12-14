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
#include "graphics_view.h"

class MainWindow : public QMainWindow {

public:
    explicit MainWindow(QWidget *parent = nullptr): QMainWindow(parent) {

        this->scene = new GraphicsView();
        this->scene->setStyleSheet("background-color: green");

        createToolBar();

        this->setCentralWidget(scene);

    }

    void createToolBar() {
        toolBar = new QToolBar;
        this->addToolBar(Qt::LeftToolBarArea, toolBar);

        position_action = new QAction("Position");
        position_action->setCheckable(true);

        connect(position_action, &QAction::toggled, this, &MainWindow::positionChecked);

        transition_action = new QAction("Transition");
        transition_action->setCheckable(true);

        connect(transition_action, &QAction::toggled, this, &MainWindow::transitionChecked);

        move_action = new QAction("Move");
        move_action->setCheckable(true);

        connect(move_action, &QAction::toggled, this, &MainWindow::moveChecked);

        toolBar->addAction(position_action);
        toolBar->addAction(transition_action);
        toolBar->addAction(move_action);
    }

public slots:

    void positionChecked(bool checked) {
        if (checked) scene->setAction(GraphicsView::A_Position);
        else scene->setAction(GraphicsView::A_Nothing);
    }

    void transitionChecked(bool checked) {
        if (checked) scene->setAction(GraphicsView::A_Transition);
        else scene->setAction(GraphicsView::A_Nothing);
    }

    void moveChecked(bool checked) {
        if (checked) scene->setAction(GraphicsView::A_Move);
        else scene->setAction(GraphicsView::A_Nothing);
    }

private:

    GraphicsView* scene = nullptr;


    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;

    QToolBar* toolBar = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H