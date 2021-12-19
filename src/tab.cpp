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

    connect(this->view, &GraphicsView::signalSceneChanged, this, &Tab::slotDocumentChanged);

}

GraphicsView *Tab::scene() {
    return this->view;
}

void Tab::slotDocumentChanged() {
    m_changed = true;
}

bool Tab::setFile(const QString& filename) {
    m_file.setFileName(filename);
    return m_file.open(QIODeviceBase::ReadWrite);
}

QFile& Tab::file() {
    return m_file;
}

void Tab::setChanged(bool changed) {
    this->m_changed = changed;
}

void Tab::closeFile() {
    m_file.close();
}
