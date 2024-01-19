#include "SynthesisModel.h"
#include <utility>

SynthesisModel::SynthesisModel(ptn::modules::decompose::DecomposeContext *ctx, QVector<size_t> programs, QObject *parent)
    : QAbstractTableModel(parent)
    , m_ctx(ctx)
    , m_programs(std::move(programs))
{

}

int SynthesisModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(m_programs.size());
}

int SynthesisModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return 1;
}

QVariant SynthesisModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        return (int)m_programs[index.row()];
    }

    return {};
}

QVariant SynthesisModel::headerData(int section, Qt::Orientation orientation, int role) const {

    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        return "Index";
    }

    return QAbstractItemModel::headerData(section, orientation, role);
}