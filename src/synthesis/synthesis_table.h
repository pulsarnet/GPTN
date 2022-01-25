//
// Created by nmuravev on 23.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_TABLE_H
#define FFI_RUST_SYNTHESIS_TABLE_H

#include <QWidget>
#include <QTableView>
#include "../ffi/rust.h"

class SynthesisTable : public QWidget {

    Q_OBJECT

public:

    explicit SynthesisTable(ffi::SynthesisContext* ctx, QWidget* parent = nullptr);


public slots:

    void slotEvalProgram(bool);

signals:

    void signalSynthesisedProgram(ffi::PetriNet*);

private:

    QTableView* m_table = nullptr;
    ffi::SynthesisContext* m_context = nullptr;
};

#endif //FFI_RUST_SYNTHESIS_TABLE_H
