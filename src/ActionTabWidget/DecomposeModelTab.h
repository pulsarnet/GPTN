//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_DECOMPOSEMODELTAB_H
#define FFI_RUST_DECOMPOSEMODELTAB_H

#include <QWidget>
#include "NetModelingTab.h"
#include "../DockSystem/DockWidget.h"
#include <DockManager.h>

class SynthesisWindow;
class QScatter3DSeries;

class DecomposeModelTab : public QWidget {

public:

    explicit DecomposeModelTab(NetModelingTab* mainTab, QWidget *parent = nullptr);

public slots:

    void selectedPoint(int idx);

private:

    NetModelingTab* m_netModelingTab;
    ffi::DecomposeContext* m_ctx;

    ads::CDockManager* m_dockManager;
    DockWidget* m_linearBaseFragmentsView;
    DockWidget* m_primitiveNetView;
    DockWidget* m_plotWidget;

    QScatter3DSeries* m_series;

    QHash<size_t, SynthesisWindow*> m_synthesisWindows;
    QHash<QVector3D, QVector<std::size_t>> m_graphPoints;
};


#endif //FFI_RUST_DECOMPOSEMODELTAB_H
