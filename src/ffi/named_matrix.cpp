//
// Created by nmuravev on 1/20/2022.
//

#include "named_matrix.h"
#include "rust.h"

NamedMatrix::NamedMatrix(FFINamedMatrix *other) {
    auto row_count = other->rows_len;
    auto column_count = other->cols_len;

    matrix.resize(row_count * column_count, 0);

    for (int i = 0; i < row_count; i++) {
        for (int j = 0; j < column_count; j++) {
            matrix[column_count * i + j] = *(other->matrix + column_count * i + j);
        }
    }

    auto cursor = other->rows;
    for (int i = 0; i < row_count; i++) {
        rows.push_back(QString(*cursor));
        cursor++;
    }

    cursor = other->cols;
    for (int i = 0; i < column_count; i++) {
        cols.push_back(QString(*cursor));
        cursor++;
    }
}
