#ifndef FFI_RUST_QWTTIMELINETRANSITIONACTIVITY_H
#define FFI_RUST_QWTTIMELINETRANSITIONACTIVITY_H

#include <QwtPlot>
#include <ptn/vertex.h>

class QwtPlotShapeItem;

namespace ptn::modules::simulation {
    struct Simulation;
}

class QwtTimeLineTransitionActivity : public QwtPlot {

public:

    explicit QwtTimeLineTransitionActivity(QWidget* parent = nullptr);

    // set net item
    void setSimulation(ptn::modules::simulation::Simulation* simulation);

    void registerFire(ptn::net::vertex::VertexIndex, int);

protected slots:

    void updatePlot();

private:

    void resetPlot();

    ptn::modules::simulation::Simulation* m_simulation;
    QTimer* m_timer;
    QHash<ptn::net::vertex::VertexIndex, QwtPlotShapeItem*> m_shapes;
    QHash<ptn::net::vertex::VertexIndex, int> m_plotMapper;
    int m_cycle;
    bool m_updated;

};


#endif //FFI_RUST_QWTTIMELINETRANSITIONACTIVITY_H
