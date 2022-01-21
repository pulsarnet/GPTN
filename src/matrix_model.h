//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_MATRIX_MODEL_H
#define FFI_RUST_MATRIX_MODEL_H

#include <QAbstractTableModel>
#include "ffi/rust.h"

class MatrixModel : public QAbstractTableModel {

public:

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override {
        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return Qt::ItemFlag::ItemIsEnabled;
    }

    int rowCount(const QModelIndex &parent) const override {
        return m_matrix->rows();
    }

    int columnCount(const QModelIndex &parent) const override {
        return m_matrix->columns();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return QString("%1").arg(m_matrix->index(index.row(), index.column()));
        }

        return QVariant();
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        return createIndex(row, column);
    }

    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override {
        return createIndex(row, column);
    }

    static MatrixModel* loadFromMatrix(ffi::CMatrix* m) {
        auto model = new MatrixModel;
        model->m_matrix = m;
        return model;
    }



private:

    ffi::CMatrix* m_matrix = nullptr;

};


#endif //FFI_RUST_MATRIX_MODEL_H
