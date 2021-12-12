//
// Created by Николай Муравьев on 11.12.2021.
//

#ifndef FFI_RUST_MAINWINDOW_H
#define FFI_RUST_MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCoreApplication>
#include <iostream>
#include "scene.h"

class MainWindow : public QMainWindow {

public:
    explicit MainWindow(QWidget *parent = nullptr): QMainWindow(parent) {
        this->scene = new Scene(this);

        this->setCentralWidget(this->scene);
    }

private Q_SLOTS:

private:

    Scene* scene = nullptr;

};

#endif //FFI_RUST_MAINWINDOW_H