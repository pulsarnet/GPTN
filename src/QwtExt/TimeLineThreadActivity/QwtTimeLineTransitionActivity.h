//
// Created by darkp on 22.10.2022.
//

#ifndef FFI_RUST_QWTTIMELINETRANSITIONACTIVITY_H
#define FFI_RUST_QWTTIMELINETRANSITIONACTIVITY_H

#include <QwtPlot>
#include "../../Core/FFI/rust.h"

class QwtPlotShapeItem;

namespace ffi {
    struct Simulation;
}

class QwtTimeLineTransitionActivity : public QwtPlot {

public:

    explicit QwtTimeLineTransitionActivity(QWidget* parent = nullptr);

    // set net item
    void setSimulation(ffi::Simulation* simulation);

    void registerFire(ffi::VertexIndex, int);

protected slots:

    void updatePlot();

private:

    void resetPlot();

private:

    ffi::Simulation* m_simulation;
    QTimer* m_timer;
    QHash<ffi::VertexIndex, QwtPlotShapeItem*> m_shapes;
    QHash<ffi::VertexIndex, int> m_plotMapper;
    int m_cycle;
    bool m_updated;

};


#endif //FFI_RUST_QWTTIMELINETRANSITIONACTIVITY_H
