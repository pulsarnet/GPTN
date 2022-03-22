//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_DECOMPOSEMODELTAB_H
#define FFI_RUST_DECOMPOSEMODELTAB_H

#include <QWidget>
#include "NetModelingTab.h"
#include "../DockSystem/DockWidget.h"
#include <DockManager.h>


class DecomposeModelTab : public QWidget {

public:

    explicit DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent = nullptr);


public slots:

    void slotSynthesisedProgram(ffi::PetriNet* net, int index);

private:

    NetModelingTab* m_netModelingTab;
    ffi::DecomposeContext* m_ctx;

    ads::CDockManager* m_dockManager;
    DockWidget* m_linearBaseFragmentsView;
    DockWidget* m_primitiveNetView;
    DockWidget* m_synthesisedProgramView;
    DockWidget* m_synthesisTable;

};


#endif //FFI_RUST_DECOMPOSEMODELTAB_H
