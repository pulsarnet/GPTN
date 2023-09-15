//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_NAMED_MATRIX_MODEL_H
#define FFI_RUST_NAMED_MATRIX_MODEL_H

#include <QAbstractTableModel>
#include "Core/FFI/rust.h"

class NamedMatrixModel : public QAbstractTableModel {

public:

    explicit NamedMatrixModel(ffi::CNamedMatrix* matrix, QObject* parent = nullptr): QAbstractTableModel(parent), m_matrix(matrix) {

    }

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override {
        Q_UNUSED(data)
        Q_UNUSED(action)
        Q_UNUSED(row)
        Q_UNUSED(column)
        Q_UNUSED(parent)
        return false;
    }

    [[nodiscard]] Qt::ItemFlags flags(const QModelIndex &index) const override {
        Q_UNUSED(index)
        return Qt::ItemFlag::ItemIsEnabled;
    }

    [[nodiscard]] int rowCount(const QModelIndex &parent) const override {
        Q_UNUSED(parent)
        return (int)m_matrix->rows();
    }

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override {
        Q_UNUSED(parent)
        return (int)m_matrix->columns();
    }

    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return m_matrix->index(index.row(), index.column());
        }

        return {};
    }

    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Orientation::Horizontal) {
                return m_matrix->horizontalHeader(section);
            }
            else {
                return m_matrix->verticalHeader(section);
            }
        }

        return {};
    }

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent) const override {
        Q_UNUSED(parent);
        return createIndex(row, column);
    }

    [[nodiscard]] QModelIndex sibling(int row, int column, const QModelIndex &idx) const override {
        Q_UNUSED(idx);
        return createIndex(row, column);
    }

private:

    ffi::CNamedMatrix* m_matrix;

};


#endif //FFI_RUST_NAMED_MATRIX_MODEL_H
