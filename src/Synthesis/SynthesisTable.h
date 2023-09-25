//
// Created by nmuravev on 23.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_TABLE_H
#define FFI_RUST_SYNTHESIS_TABLE_H

#include <QWidget>

namespace ffi {
    struct PetriNet;
    struct DecomposeContext;
}

class QAction;
class QTableView;
class Switch;
class SynthesisModel;

class SynthesisTable : public QWidget {

    Q_OBJECT

public:

    explicit SynthesisTable(ffi::DecomposeContext* ctx, QVector<size_t> programs, QWidget* parent = nullptr);

    SynthesisModel* model() const;

    QTableView* table();
    const QTableView* table() const;
private:

    QTableView* m_table = nullptr;
};

#endif //FFI_RUST_SYNTHESIS_TABLE_H
