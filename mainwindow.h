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
#include <QActionGroup>
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

        QActionGroup* actionGroup = new QActionGroup(toolBar);

        position_action = new QAction("Position");
        position_action->setIcon(QIcon(":/images/circle.png"));
        position_action->setCheckable(true);

        connect(position_action, &QAction::toggled, this, &MainWindow::positionChecked);

        transition_action = new QAction("Transition");
        transition_action->setIcon(QIcon(":/images/rectangle.png"));
        transition_action->setCheckable(true);

        connect(transition_action, &QAction::toggled, this, &MainWindow::transitionChecked);

        move_action = new QAction("Move");
        move_action->setIcon(QIcon(":/images/move.png"));
        move_action->setCheckable(true);

        connect(move_action, &QAction::toggled, this, &MainWindow::moveChecked);

        connect_action = new QAction("Connect");
        connect_action->setIcon(QIcon(":/images/connect.png"));
        connect_action->setCheckable(true);

        connect(connect_action, &QAction::toggled, this, &MainWindow::connectChecked);

        actionGroup->addAction(position_action);
        actionGroup->addAction(transition_action);
        actionGroup->addAction(move_action);
        actionGroup->addAction(connect_action);

        toolBar->addAction(position_action);
        toolBar->addAction(transition_action);
        toolBar->addAction(move_action);
        toolBar->addAction(connect_action);
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

    void connectChecked(bool checked) {
        if (checked) scene->setAction(GraphicsView::A_Connect);
        else scene->setAction(GraphicsView::A_Nothing);
    }

private:

    GraphicsView* scene = nullptr;

    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;
    QAction* connect_action = nullptr;

    QToolBar* toolBar = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H