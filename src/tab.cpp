//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include "../include/tab.h"

#include "../include/mainwindow.h"
#include "../include/synthesis/synthesis_view.h"
#include "../include/ffi/rust.h"

Tab::Tab(QWidget *parent) : QWidget(parent) {

    m_net = PetriNet::make();

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    this->edit_view = new GraphicsView(this);
    this->edit_view->setWindowTitle("Main view");

    this->primitive_view = new GraphicsView(this);
    this->primitive_view->setWindowTitle("Primitive View");

    this->lbf_view = new GraphicsView(this);
    this->lbf_view->setWindowTitle("Logical Base Fragments View");

    //this->edit_view->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred));
    layout->addWidget(this->edit_view, 0, 0, 1, 2);
    layout->addWidget(this->primitive_view, 1, 0, 1, 1);
    layout->addWidget(this->lbf_view, 1, 1, 1, 1);

    connect(this->edit_view, &GraphicsView::signalSceneChanged, this, &Tab::slotDocumentChanged);
    connect(this->edit_view, &GraphicsView::signalRemoveItem, this, &Tab::slotRemoveItem);

    this->m_split_actions = new SplitListModel();
    this->setLayout(layout);

}

GraphicsView *Tab::scene() {
    return this->edit_view;
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

    auto synthesis_program = split_net(this->edit_view->net());

    SynthesisView* view = new SynthesisView(synthesis_program, this);
    view->show();

}

void Tab::slotRemoveItem() {
    this->m_split_actions->clear();
}
