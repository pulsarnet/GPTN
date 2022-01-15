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

    auto primitive_scene = new GraphicScene;
    primitive_scene->setAllowMods(GraphicScene::A_Nothing);
    this->primitive_view = new GraphicsView(this);
    this->primitive_view->setWindowTitle("Primitive View");
    this->primitive_view->setScene(primitive_scene);

    auto lbf_scene = new GraphicScene;
    lbf_scene->setAllowMods(GraphicScene::A_Nothing);
    this->lbf_view = new GraphicsView(this);
    this->lbf_view->setWindowTitle("Logical Base Fragments View");
    this->lbf_view->setScene(lbf_scene);

    layout->addWidget(this->edit_view, 0, 0, 1, 2);
    layout->addWidget(this->primitive_view, 1, 0, 1, 1);
    layout->addWidget(this->lbf_view, 1, 1, 1, 1);

    this->m_split_actions = new SplitListModel();
    this->setLayout(layout);

    connect(main_scene, &QGraphicsScene::changed, this, &Tab::slotDocumentChanged);

}

void Tab::splitAction() {

    auto synthesis_program = split_net(qobject_cast<GraphicScene*>(this->edit_view->scene())->net());
    // Добавим пустую программу чтобы не падало
    synthesis_program->add_program();
    auto result = split_finish(synthesis_program);

    auto scene = dynamic_cast<GraphicScene*>(primitive_view->scene());
    scene->removeAll();

    auto primitive_matrix = result.lbf_matrix;
    auto transitions_count = primitive_matrix.cols.count();
    int tran_rows = round((double)transitions_count / 3.);
    int tran_cols = 3;

    qDebug() << tran_rows << tran_cols;

    int start_x = 0;
    int start_y = 0;
    int offset_x = 120;
    int offset_y = 120;

    for (int i = 0; i < tran_rows; i++) {
        for (int j = 0; j < tran_cols; j++) {

            auto transition = i * tran_cols + j;

            if (transition >= primitive_matrix.cols.count()) break;

            PetriObject *first, *second;

            for (int p = 0; p < primitive_matrix.rows.count(); p++) {
                auto tmp = primitive_matrix(p, transition);
                if (tmp < 0) {
                    first = scene->addPosition(primitive_matrix.rows[p], QPointF(start_x, start_y));
                }
                else if (tmp > 0) {
                    second = scene->addPosition(primitive_matrix.rows[p], QPointF(start_x + offset_x * 2, start_y));
                }
            }

            auto center =
                    scene->addTransition(primitive_matrix.cols[transition], QPoint(start_x + offset_x, start_y));

            scene->connectItems(first, center);
            scene->connectItems(center, second);

            start_x += offset_x * 3 + 20;

        }

        start_x = 0;
        start_y += offset_y;
    }

//    auto view = new SynthesisView(synthesis_program, this);
//    view->show();

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
    qobject_cast<GraphicScene*>(this->edit_view->scene())->fromVariant(data);
    this->edit_view->fitInView(this->edit_view->scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}
