//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include "../include/tab.h"

#include "../include/mainwindow.h"
#include "../include/synthesis/synthesis_view.h"
#include "../include/ffi/rust.h"

Tab::Tab(QWidget *parent) : QWidget(parent) {

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->view = new GraphicsView(this);
    this->view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    layout->addWidget(this->view);

    connect(this->view, &GraphicsView::signalSceneChanged, this, &Tab::slotDocumentChanged);
    connect(this->view, &GraphicsView::signalRemoveItem, this, &Tab::slotRemoveItem);

    this->m_split_actions = new SplitListModel();

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

void Tab::splitAction() {

    auto synthesis_program = split_net(this->view->net());

    SynthesisView* view = new SynthesisView(synthesis_program, this);
    view->show();

}

void Tab::slotRemoveItem() {
    this->m_split_actions->clear();
}
