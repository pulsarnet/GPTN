//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_DECOMPOSEMODELTAB_H
#define FFI_RUST_DECOMPOSEMODELTAB_H

#include <QWidget>
#include "NetModelingTab.h"

class GraphicsView;
class SynthesisTable;

class DecomposeModelTab : public QWidget {

public:

    explicit DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent = nullptr);


public slots:

    void slotSynthesisedProgram(ffi::PetriNet* net, int index);

private:

    NetModelingTab* m_netModelingTab;
    ffi::DecomposeContext* m_ctx;

    GraphicsView* m_linearBaseFragmentsView;
    GraphicsView* m_primitiveNetView;
    GraphicsView* m_synthesisedProgramView;

    SynthesisTable* m_synthesisTable;

};


#endif //FFI_RUST_DECOMPOSEMODELTAB_H
