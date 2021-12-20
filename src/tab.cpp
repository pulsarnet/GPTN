//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include "../include/tab.h"
#include "../include/rust.h"

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

void Tab::splitAction() {

    auto petri = make();

    for (auto item : this->view->getItems()) {
        auto petriItem = dynamic_cast<PetriObject*>(item);
        if (petriItem->objectType() == PetriObject::Position) {
            add_position(petri, petriItem->index());
        }
        else if (petriItem->objectType() == PetriObject::Transition) {
            add_transition(petri, petriItem->index());
        }
    }

    for (auto conn : this->view->getConnections()) {
        if (conn.second.first->objectType() == PetriObject::Position) {
            connect_p(petri, conn.second.first->index(), conn.second.second->index());
        }
        else if (conn.second.first->objectType() == PetriObject::Transition) {
            connect_t(petri, conn.second.first->index(), conn.second.second->index());
        }
    }

    split(petri);
    del(petri);

}
