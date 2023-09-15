//
// Created by darkp on 20.10.2022.
//
// Действия
// 1. Запуск симуляции
// 2. Пауза
// 3. Остановка
// 4. Шаг симуляции
// 5. Ускорение
// 6. Замедление
// 7. Счетчик циклов

#ifndef FFI_RUST_SIMULATIONWIDGET_H
#define FFI_RUST_SIMULATIONWIDGET_H

#include <QFrame>
#include "../../Core/FFI/rust.h"

class QLabel;
class QPushButton;
class GraphicsView;
class QwtTimeLineTransitionActivity;

namespace ffi {
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

    void updatePlot();
    void openPlot();
    void closePlot();

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

    ffi::Simulation* m_simulation;

    QwtTimeLineTransitionActivity* m_plot;

};


#endif //FFI_RUST_SIMULATIONWIDGET_H
