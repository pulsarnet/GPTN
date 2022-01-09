//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include "../include/tab.h"

#include "../include/mainwindow.h"
#include "../include/synthesis/synthesis_view.h"
#include "../include/rust.h"

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
        if (conn->from()->objectType() == PetriObject::Position) {
            connect_p(petri, conn->from()->index(), conn->to()->index());
        }
        else if (conn->from()->objectType() == PetriObject::Transition) {
            connect_t(petri, conn->from()->index(), conn->to()->index());
        }
    }

    this->m_split_actions->clear();

    auto synthesis_program = split_net(petri);

    SynthesisView* view = new Synthe2sisView(synthesis_program, this);
    view->show();


    del(petri);

}

void Tab::slotRemoveItem() {
    this->m_split_actions->clear();
}
