#ifndef FFI_RUST_MATRIXWINDOW_H
#define FFI_RUST_MATRIXWINDOW_H

#include <QWidget>
#include <QDialog>
#include <ptn/matrix.h>
#include <ptn/types.h>

class QTableView;
class QGridLayout;

class MatrixWindow : public QDialog {

    Q_OBJECT

public:

    explicit MatrixWindow(ptn::matrix::RustMatrix<i32>&& matrix1, ptn::matrix::RustMatrix<i32>&& matrix2, QWidget* parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

signals:

    void onWindowClose(QWidget*);

private:

    QTableView* m_matrix1;
    QTableView* m_matrix2;
    QGridLayout* m_layout;
};


#endif //FFI_RUST_MATRIXWINDOW_H
