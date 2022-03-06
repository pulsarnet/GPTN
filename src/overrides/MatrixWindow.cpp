//
// Created by nmuravev on 06.03.2022.
//

#include <QLabel>
#include <QHeaderView>
#include "MatrixWindow.h"
#include "../named_matrix_model.h"

MatrixWindow::MatrixWindow(CNamedMatrix *matrix1, CNamedMatrix *matrix2, QWidget *parent) : QWidget(parent),
    m_layout(new QGridLayout(this))
{
    setWindowTitle("Hello world");
    setEnabled(true);

    m_matrix1 = new QTableView;
    m_matrix1->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix1->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix1->setModel(new NamedMatrixModel(matrix1));

    m_matrix2 = new QTableView;
    m_matrix2->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix2->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix2->setModel(new NamedMatrixModel(matrix2));

    auto m_matrixLabel1 = new QLabel("Input", this);
    auto m_matrixLabel2 = new QLabel("Output", this);

    m_layout->addWidget(m_matrixLabel1, 0, 0);
    m_layout->addWidget(m_matrixLabel2, 0, 1);
    m_layout->addWidget(m_matrix1, 1, 0);
    m_layout->addWidget(m_matrix2, 1, 1);
}