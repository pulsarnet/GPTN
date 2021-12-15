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

        createToolBar();

        this->setCentralWidget(scene);
    }

    QAction* makeAction(const QString& name, const QIcon& icon, bool checkable, QActionGroup* actionGroup = nullptr) {
        auto action = new QAction(name);
        action->setIcon(icon);
        action->setCheckable(checkable);

        if (actionGroup) actionGroup->addAction(action);

        return action;
    }

    void createToolBar() {
        toolBar = new QToolBar;
        this->addToolBar(Qt::LeftToolBarArea, toolBar);

        QActionGroup* actionGroup = new QActionGroup(toolBar);

        position_action = makeAction("Position", QIcon(":/images/circle.png"), true, actionGroup);
        transition_action = makeAction("Transition", QIcon(":/images/rectangle.png"), true, actionGroup);
        move_action = makeAction("Move", QIcon(":/images/move.png"), true, actionGroup);
        connect_action = makeAction("Connect", QIcon(":/images/connect.png"), true, actionGroup);
        rotation_action = makeAction("Rotate", QIcon(":/images/rotation.png"), true, actionGroup);


        connect(position_action, &QAction::toggled, this, &MainWindow::positionChecked);
        connect(transition_action, &QAction::toggled, this, &MainWindow::transitionChecked);
        connect(move_action, &QAction::toggled, this, &MainWindow::moveChecked);
        connect(connect_action, &QAction::toggled, this, &MainWindow::connectChecked);
        connect(rotation_action, &QAction::toggled, this, &MainWindow::rotateChecked);


        toolBar->addAction(position_action);
        toolBar->addAction(transition_action);
        toolBar->addAction(move_action);
        toolBar->addAction(connect_action);
        toolBar->addAction(rotation_action);
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

    void rotateChecked(bool checked) {
        if(checked) scene->setAction(GraphicsView::A_Rotate);
        else scene->setAction(GraphicsView::A_Nothing);
    }

private:

    GraphicsView* scene = nullptr;

    QAction* position_action = nullptr;
    QAction* transition_action = nullptr;
    QAction* move_action = nullptr;
    QAction* connect_action = nullptr;
    QAction* rotation_action = nullptr;

    QToolBar* toolBar = nullptr;
};

#endif //FFI_RUST_MAINWINDOW_H