//
// Created by nmuravev on 1/20/2022.
//

#include "synthesis_programs_model.h"
#include "../ffi/rust.h"
#include "column_delegate_type.h"

bool SynthesisProgramModel::dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                                         const QModelIndex &parent) {
    return false;
}

Qt::ItemFlags SynthesisProgramModel::flags(const QModelIndex &index) const {
    return Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsEditable;
}

int SynthesisProgramModel::rowCount(const QModelIndex &parent) const {
    return m_program->programs();
}

int SynthesisProgramModel::columnCount(const QModelIndex &parent) const {
    return m_program->transitions() + m_program->positions() + 1;
}

QVariant SynthesisProgramModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        if (index.column() == (columnCount(QModelIndex()) - 1)) return QVariant();
        return QString("%1").arg(m_program->get_program_value(index.row(), index.column()));
    }
    else if (role == Qt::UserRole) {
        if (index.column() == (columnCount(QModelIndex()) - 1)) return ColumnDelegateType::ShowButton;
        else return ColumnDelegateType::Value;
    }
    else if (role == ProgramItemDelegate::EvalProgramRole) {
        auto res = m_program->eval(index.row());
    }

    return QVariant();
}

QVariant SynthesisProgramModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == (columnCount(QModelIndex()) - 1)) return QVariant();
        if (section <= (m_program->transitions() - 1)) {
            return m_program->transition(section);
        }
        return m_program->position(section - m_program->transitions());
    }

    return QVariant();
}
