#include <QLabel>
#include <QHeaderView>
#include "MatrixWindow.h"
#include "../NamedMatrixModel.h"
#include <QGridLayout>
#include <QTableView>

MatrixWindow::MatrixWindow(ptn::matrix::RustMatrix<i32>&& matrix1, ptn::matrix::RustMatrix<i32>&& matrix2, QWidget *parent)
    : QDialog(parent)
    , m_layout(new QGridLayout(this))
{
    setWindowTitle("I/O Matrix View");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_DeleteOnClose);
    setEnabled(true);
    setModal(false);

    m_matrix1 = new QTableView;
    m_matrix1->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix1->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix1->setModel(new NamedMatrixModel(std::move(matrix1)));

    m_matrix2 = new QTableView;
    m_matrix2->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix2->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_matrix2->setModel(new NamedMatrixModel(std::move(matrix2)));

    auto m_matrixLabel1 = new QLabel("Input", this);
    auto m_matrixLabel2 = new QLabel("Output", this);

    m_layout->addWidget(m_matrixLabel1, 0, 0);
    m_layout->addWidget(m_matrixLabel2, 0, 1);
    m_layout->addWidget(m_matrix1, 1, 0);
    m_layout->addWidget(m_matrix2, 1, 1);
}

void MatrixWindow::closeEvent(QCloseEvent *event) {
    emit onWindowClose(this);
    QWidget::closeEvent(event);
}