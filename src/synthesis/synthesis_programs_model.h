//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_MODEL_H
#define FFI_RUST_SYNTHESIS_PROGRAM_MODEL_H

#include <QAbstractTableModel>
#include "../ffi/named_matrix.h"
#include "synthesis_program.h"
#include "synthesis_program_item_delegate.h"


class SynthesisProgramModel : public QAbstractTableModel {

public:

    explicit SynthesisProgramModel(SynthesisProgram* program, QObject* parent = nullptr) : QAbstractTableModel(parent),
                                                                                        m_program(program)
    {

    }

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent) const override;

    int columnCount(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (role == Qt::EditRole) {
            if (index.column() == (columnCount(QModelIndex()) - 1)) return true;
            m_program->set_program_value(index.row(), index.column(), value.toInt());
            return true;
        }
        return QAbstractItemModel::setData(index, value, role);
    }

    bool insertRows(int row, int count, const QModelIndex &parent) override {
        beginInsertRows(QModelIndex(), row, row + count - 1);

        m_program->add_program();

        endInsertRows();

        return true;
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        return createIndex(row, column);
    }

    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override {
        return createIndex(row, column);
    }

    SynthesisProgram* program() {
        return m_program;
    }

private:

    SynthesisProgram* m_program = nullptr;

};


#endif //FFI_RUST_NAMED_MATRIX_MODEL_H
