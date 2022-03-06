//
// Created by nmuravev on 06.03.2022.
//

#ifndef FFI_RUST_MATRIXWINDOW_H
#define FFI_RUST_MATRIXWINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QTableView>
#include "../ffi/rust.h"

using namespace ffi;

class MatrixWindow : public QWidget {

public:

    explicit MatrixWindow(CNamedMatrix* matrix1, CNamedMatrix* matrix2, QWidget* parent = nullptr);


private:

    QTableView* m_matrix1;
    QTableView* m_matrix2;
    QGridLayout* m_layout;
};


#endif //FFI_RUST_MATRIXWINDOW_H
