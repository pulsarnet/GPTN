//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_NAMED_MATRIX_H
#define FFI_RUST_NAMED_MATRIX_H

#include <QList>

struct NamedMatrix {
    QList<QString> rows;
    QList<QString> cols;

    QList<long> matrix;

    long operator()(int i, int j) const {
        return matrix[cols.length() * i + j];
    }
};

#endif //FFI_RUST_NAMED_MATRIX_H
