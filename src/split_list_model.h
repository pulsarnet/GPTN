//
// Created by nmuravev on 12/20/2021.
//

#ifndef FFI_RUST_SPLIT_LIST_MODEL_H
#define FFI_RUST_SPLIT_LIST_MODEL_H

#include <QAbstractListModel>
#include <utility>

class SplitListModel : public QAbstractListModel {

public:

    explicit SplitListModel(QObject* parent = nullptr) : QAbstractListModel(parent) {

    }

    void clear() { this->m_actions.clear(); }

    void setActions(QList<QList<QString>> actions) {
        beginResetModel();
        this->m_actions = std::move(actions);
        endResetModel();
    }

    int rowCount(const QModelIndex &parent) const override {
        return m_actions.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return QString(m_actions[index.row()][0]);
        }
        else if (role == Qt::UserRole) {
            return m_actions[index.row()];
        }
        return QVariant();
    }

private:

    QList<QList<QString>> m_actions;

};

#endif //FFI_RUST_SPLIT_LIST_MODEL_H
