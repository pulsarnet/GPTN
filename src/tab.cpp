//
// Created by nmuravev on 12/16/2021.
//

#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>

#include "../include/tab.h"

#include "../include/mainwindow.h"
#include "../include/synthesis/synthesis_view.h"
#include "../include/ffi/rust.h"

Tab::Tab(QWidget *parent) : QWidget(parent) {

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto main_scene = new GraphicScene;
    main_scene->setAllowMods(GraphicScene::A_Default);
    this->edit_view = new GraphicsView(this);
    this->edit_view->setWindowTitle("Main view");
    this->edit_view->setScene(main_scene);

    this->primitive_view = new GraphicsView(this);
    this->primitive_view->setWindowTitle("Primitive View");

    this->lbf_view = new GraphicsView(this);
    this->lbf_view->setWindowTitle("Logical Base Fragments View");

    layout->addWidget(this->edit_view, 0, 0, 1, 2);
    layout->addWidget(this->primitive_view, 1, 0, 1, 1);
    layout->addWidget(this->lbf_view, 1, 1, 1, 1);

    this->m_split_actions = new SplitListModel();
    this->setLayout(layout);

    connect(main_scene, &QGraphicsScene::changed, this, &Tab::slotDocumentChanged);

}

void Tab::splitAction() {

    auto synthesis_program = split_net(qobject_cast<GraphicScene*>(this->edit_view->scene())->net());

    auto view = new SynthesisView(synthesis_program, this);
    view->show();

}

void Tab::saveToFile() {
    auto filename = !m_filename.isEmpty() ? m_filename :
            QFileDialog::getSaveFileName(this, tr("Save project file"), tr("Petri network (*.ptn)"));

    if (filename.isEmpty())
        return;
    else {
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_6_0);
        stream << this->toData();

        setFileName(filename);
    }
}

void Tab::loadFromFile() {

    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Open Petri Network project"), "",
                                                    tr("Petri Network File (*.ptn)"));

    if (filename.isEmpty())
        return;
    else {
        QFile file(filename);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::information(this, tr("Unable to open file"), file.errorString());
            return;
        }

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_6_0);

        QVariant data;
        stream >> data;
        fromData(data);

        setFileName(filename);
    }

}

QVariant Tab::toData() {
    return qobject_cast<GraphicScene*>(this->edit_view->scene())->toVariant();
}

void Tab::setFileName(QString filename) {

    m_filename = filename;

    if (m_filename.isEmpty()) this->setWindowTitle("Untitled");
    else this->setWindowTitle(QFileInfo(filename).fileName());

}

void Tab::fromData(QVariant data) {
    return qobject_cast<GraphicScene*>(this->edit_view->scene())->fromVariant(data);
}
