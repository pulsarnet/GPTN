//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_MATRIX_H
#define FFI_RUST_MATRIX_H

#include <QList>

struct FFIMatrix;

struct Matrix {
    unsigned long rows = 0;
    unsigned long cols = 0;

    QList<long> matrix;

    Matrix() = default;
    explicit Matrix(FFIMatrix* matrix);

    long operator()(int i, int j) const {
        return matrix[cols * i + j];
    }
};

#endif //FFI_RUST_MATRIX_H
