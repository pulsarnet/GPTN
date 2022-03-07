//
// Created by nmuravev on 06.03.2022.
//

#ifndef FFI_RUST_MATRIXWINDOW_H
#define FFI_RUST_MATRIXWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QTableView>
#include "../ffi/rust.h"

class MatrixWindow : public QWidget {

    Q_OBJECT

public:

    explicit MatrixWindow(ffi::CNamedMatrix* matrix1, ffi::CNamedMatrix* matrix2, QWidget* parent = nullptr);

    void closeEvent(QCloseEvent *event) override;

signals:

    void onWindowClose(QWidget*);

private:

    QTableView* m_matrix1;
    QTableView* m_matrix2;
    QGridLayout* m_layout;
};


#endif //FFI_RUST_MATRIXWINDOW_H
