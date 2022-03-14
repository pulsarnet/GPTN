//
// Created by nmuravev on 23.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_MODEL_H
#define FFI_RUST_SYNTHESIS_MODEL_H

#include <QAbstractTableModel>

namespace ffi {
    struct DecomposeContext;
}

class SynthesisModel : public QAbstractTableModel {

public:

    explicit SynthesisModel(ffi::DecomposeContext* ctx, QObject* parent = nullptr);

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool insertRows(int row, int count, const QModelIndex &parent) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    ffi::DecomposeContext* ctx() {
        return m_ctx;
    }

public slots:

    void slotEnableLabels(bool checked);

private:

    ffi::DecomposeContext* m_ctx = nullptr;
    bool m_enableLabels = false;

};


#endif //FFI_RUST_SYNTHESIS_MODEL_H
