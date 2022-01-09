//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_NAMED_MATRIX_MODEL_H
#define FFI_RUST_NAMED_MATRIX_MODEL_H

#include <QAbstractTableModel>
#include "named_matrix.h"

class NamedMatrixModel : public QAbstractTableModel {

public:

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override {
        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return Qt::ItemFlag::ItemIsEditable | Qt::ItemFlag::ItemIsEnabled;
    }

    int rowCount(const QModelIndex &parent) const override {
        return m_matrix.rows.length();
    }

    int columnCount(const QModelIndex &parent) const override {
        return m_matrix.cols.length();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return QString("%1").arg(m_matrix(index.row(), index.column()));
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role == Qt::DisplayRole) {
            if (orientation == Qt::Orientation::Horizontal) {
                return m_matrix.cols[section];
            }
            else {
                return m_matrix.rows[section];
            }
        }

        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (role == Qt::EditRole) {
            m_matrix(index.row(), index.column()) = value.toInt();
            return true;
        }
        return QAbstractItemModel::setData(index, value, role);
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        return createIndex(row, column);
    }

    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override {
        return createIndex(row, column);
    }

    static NamedMatrixModel* loadFromMatrix(const NamedMatrix& m) {
        auto model = new NamedMatrixModel;
        model->m_matrix = m;
        return model;
    }



private:

    NamedMatrix m_matrix = NamedMatrix{};

};


#endif //FFI_RUST_NAMED_MATRIX_MODEL_H
