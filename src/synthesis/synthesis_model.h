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

    explicit SynthesisModel(ffi::DecomposeContext* ctx, QVector<size_t>, QObject* parent = nullptr);

    int columnCount(const QModelIndex &parent) const override;

    int rowCount(const QModelIndex &parent) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    ffi::DecomposeContext* ctx() {
        return m_ctx;
    }

private:

    ffi::DecomposeContext* m_ctx;
    QVector<size_t> m_programs;

};


#endif //FFI_RUST_SYNTHESIS_MODEL_H
