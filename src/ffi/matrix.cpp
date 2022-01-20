//
// Created by nmuravev on 1/20/2022.
//

#include "matrix.h"
#include "rust.h"

Matrix::Matrix(FFIMatrix *ffi) {
    QList<long> elements;
    this->rows = ffi->rows_len;
    this->cols = ffi->cols_len;

    elements.resize(rows * cols, 0);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            elements[cols * i + j] = *(ffi->matrix + cols * i + j);
        }
    }

    this->matrix = std::move(elements);
}