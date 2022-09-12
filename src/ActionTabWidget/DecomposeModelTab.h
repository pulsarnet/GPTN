//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_DECOMPOSEMODELTAB_H
#define FFI_RUST_DECOMPOSEMODELTAB_H

#include <QWidget>
#include "NetModelingTab.h"
#include "../DockSystem/DockWidget.h"
#include <DockManager.h>

class QwtExtPlotCurveLabels;

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

    QwtExtPlotCurveLabels* m_plot;
};


#endif //FFI_RUST_DECOMPOSEMODELTAB_H
