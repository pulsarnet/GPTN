#ifndef FFI_RUST_SIMULATIONWIDGET_H
#define FFI_RUST_SIMULATIONWIDGET_H

#include <QFrame>

class QLabel;
class QPushButton;
class GraphicsView;
class QwtTimeLineTransitionActivity;

namespace ptn::modules::simulation {
    struct Simulation;
}

class SimulationWidget : public QFrame {

    Q_OBJECT

public:

    enum State {
        Running = 0,
        Paused,
        Stopped
    };

public:

    explicit SimulationWidget(GraphicsView* parent);

    void paintEvent(QPaintEvent *event) override;

    State state() const;

    QwtTimeLineTransitionActivity* plot() { return m_plot; }

public slots:

    void runSimulation();
    void pauseSimulation();
    void stopSimulation();
    void stepSimulation();
    void speedUp();
    void speedDown();

private:

    void updateButtonState();
    void updateLabel();
    void updateScene();

    void initSimulation();
    void simulate();
    void cancelSimulation();

    void updatePlot() const;

private:

    QTimer* m_timer;

    QPushButton* m_runButton;
    QPushButton* m_pauseButton;
    QPushButton* m_stopButton;
    QPushButton* m_stepButton;
    QPushButton* m_speedUpButton;
    QPushButton* m_speedDownButton;

    QLabel* m_cycleCounterLabel;

    State m_state;

    ptn::modules::simulation::Simulation* m_simulation;

    QwtTimeLineTransitionActivity* m_plot;

};


#endif //FFI_RUST_SIMULATIONWIDGET_H
