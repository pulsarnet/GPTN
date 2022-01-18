//
// Created by Николай Муравьев on 08.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_MODEL_H
#define FFI_RUST_SYNTHESIS_PROGRAM_MODEL_H

#include <QAbstractTableModel>
#include "../named_matrix.h"
#include "synthesis_program.h"

class SynthesisProgramModel : public QAbstractTableModel {

public:

    explicit SynthesisProgramModel(SynthesisProgram* program, QObject* parent = nullptr) : QAbstractTableModel(parent),
                                                                                        m_program(program)
    {

    }

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override {
        return false;
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsEditable;
    }

    int rowCount(const QModelIndex &parent) const override {
        return m_program->programs();
    }

    int columnCount(const QModelIndex &parent) const override {
        return m_program->transitions() + m_program->positions();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return QString("%1").arg(m_program->get_program_value(index.row(), index.column()));
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
            if (section <= (m_program->transitions() - 1)) {
                return m_program->transition(section);
            }
            return m_program->position(section - m_program->transitions());
        }

        return QVariant();
    }

    bool setData(const QModelIndex &index, const QVariant &value, int role) override {
        if (role == Qt::EditRole) {
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
