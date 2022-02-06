//
// Created by nmuravev on 23.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_TABLE_H
#define FFI_RUST_SYNTHESIS_TABLE_H

#include <QWidget>

namespace ffi {
    struct PetriNet;
    struct SynthesisContext;
}

class QToolBar;
class QAction;
class QTableView;
class Switch;

class SynthesisTable : public QWidget {

    Q_OBJECT

public:

    explicit SynthesisTable(ffi::SynthesisContext* ctx, QWidget* parent = nullptr);


public slots:

    void slotEvalProgram(bool);
    void slotAddProgram(bool);

signals:

    void signalSynthesisedProgram(ffi::PetriNet*);

private:

    QTableView* m_table = nullptr;
    ffi::SynthesisContext* m_context = nullptr;

    QToolBar* m_toolBar = nullptr;
    QAction* m_evalProgram = nullptr;
    QAction* m_addProgram = nullptr;
    Switch* m_enableLabel = nullptr;
};

#endif //FFI_RUST_SYNTHESIS_TABLE_H
