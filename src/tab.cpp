//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include "../include/tab.h"

Tab::Tab(QWidget *parent) : QWidget(parent) {

    QVBoxLayout* layout = new QVBoxLayout(this);

    this->view = new GraphicsView;
    this->view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(this->view);

}

GraphicsView *Tab::scene() {
    return this->view;
}