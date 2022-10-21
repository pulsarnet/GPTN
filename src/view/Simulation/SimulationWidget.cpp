#include "SimulationWidget.h"
#include "../GraphicsView.h"
#include "../../ffi/Simulation.h"
#include "../GraphicScene.h"
#include "../elements/position.h"
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QStyleOptionFrame>

SimulationWidget::SimulationWidget(GraphicsView *parent)
    : QFrame(parent)
    , m_timer(new QTimer(this))
    , m_cycles(0)
    , m_state(State::Stopped)
    , m_simulation(nullptr)
{

    setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    setStyleSheet("SimulationWidget {"
                  "background: #FFFFFF;"
                  "border-radius: 16px;"
                  "}");
    setFrameStyle(StyledPanel | Plain);
    //shadow
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(10);
    shadow->setOffset(0, 0);
    shadow->setColor(QColor(0, 0, 0, 150));
    setGraphicsEffect(shadow);

    //layout
    auto* layout = new QHBoxLayout(this);
    layout->setSpacing(3);
    layout->setSizeConstraint(QLayout::SetMinimumSize);

    m_runButton = new QPushButton(this);
    m_runButton->setIcon(QIcon(":/images/simulation/run.svg"));
    m_runButton->setMaximumSize(30, 30);
    m_runButton->setMinimumSize(30, 30);
    m_runButton->setCheckable(true);
    layout->addWidget(m_runButton);

    m_pauseButton = new QPushButton(this);
    m_pauseButton->setIcon(QIcon(":/images/simulation/pause.svg"));
    m_pauseButton->setMaximumSize(30, 30);
    m_pauseButton->setMinimumSize(30, 30);
    m_pauseButton->setCheckable(true);
    layout->addWidget(m_pauseButton);

    m_stopButton = new QPushButton(this);
    m_stopButton->setIcon(QIcon(":/images/simulation/stop.svg"));
    m_stopButton->setMaximumSize(30, 30);
    m_stopButton->setMinimumSize(30, 30);
    layout->addWidget(m_stopButton);

    m_stepButton = new QPushButton(this);
    m_stepButton->setIcon(QIcon(":/images/simulation/step.svg"));
    m_stepButton->setMaximumSize(30, 30);
    m_stepButton->setMinimumSize(30, 30);
    layout->addWidget(m_stepButton);

    m_speedDownButton = new QPushButton(this);
    m_speedDownButton->setIcon(QIcon(":/images/simulation/speeddown.svg"));
    m_speedDownButton->setMaximumSize(30, 30);
    m_speedDownButton->setMinimumSize(30, 30);
    layout->addWidget(m_speedDownButton);

    m_speedUpButton = new QPushButton(this);
    m_speedUpButton->setIcon(QIcon(":/images/simulation/speedup.svg"));
    m_speedUpButton->setMaximumSize(30, 30);
    m_speedUpButton->setMinimumSize(30, 30);
    layout->addWidget(m_speedUpButton);

    m_cycleCounterLabel = new QLabel(this);
    m_cycleCounterLabel->setFont(QFont("JetBrains Mono", 14));
    layout->addWidget(m_cycleCounterLabel);

    updateLabel();

    connect(m_runButton, &QPushButton::clicked, this, &SimulationWidget::runSimulation);
    connect(m_pauseButton, &QPushButton::clicked, this, &SimulationWidget::pauseSimulation);
    connect(m_stopButton, &QPushButton::clicked, this, &SimulationWidget::stopSimulation);
    connect(m_stepButton, &QPushButton::clicked, this, &SimulationWidget::stepSimulation);
    connect(m_speedUpButton, &QPushButton::clicked, this, &SimulationWidget::speedUp);
    connect(m_speedDownButton, &QPushButton::clicked, this, &SimulationWidget::speedDown);

    updateButtonState();

    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &SimulationWidget::simulate);
}

void SimulationWidget::paintEvent(QPaintEvent *event) {
    QStyleOptionFrame opt;
    opt.initFrom(this);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, this);
}

SimulationWidget::State SimulationWidget::state() const {
    return m_state;
}

void SimulationWidget::runSimulation() {
    // Запуск симуляции
    // Создание объекта ffi симуляции сети
    if (m_state != State::Paused) {
        this->initSimulation();
    }

    m_timer->start();

    m_state = State::Running;
    updateButtonState();
}

void SimulationWidget::pauseSimulation() {
    // Пауза симуляции
    // Остановка симуляции

    m_timer->stop();

    m_state = State::Paused;
    updateButtonState();
}

void SimulationWidget::stopSimulation() {
    // Остановка симуляции
    // Очистка симуляции

    m_timer->stop();
    this->cancelSimulation();

    m_state = State::Stopped;
    updateButtonState();
}

void SimulationWidget::stepSimulation() {
    // Шаг симуляции
    // Выполнение одного цикла симуляции
    if (m_state != State::Paused) {
        this->initSimulation();
    }
    this->simulate();

    m_state = State::Paused;
    updateButtonState();
}

void SimulationWidget::speedUp() {
    // Ускорение симуляции
    // Увеличение скорости симуляции
    m_timer->setInterval(m_timer->interval() / 2);
}

void SimulationWidget::speedDown() {
    // Замедление симуляции
    // Уменьшение скорости симуляции
    m_timer->setInterval(m_timer->interval() * 2);
}

void SimulationWidget::updateButtonState() {
    switch (m_state) {
        case Stopped:
            m_runButton->setChecked(false);
            m_pauseButton->setChecked(false);

            m_runButton->setEnabled(true);
            m_pauseButton->setEnabled(false);
            m_stopButton->setEnabled(false);
            m_stepButton->setEnabled(true);
            m_speedDownButton->setEnabled(true);
            m_speedUpButton->setEnabled(true);
            break;
        case Running:
            m_pauseButton->setChecked(false);

            m_runButton->setEnabled(false);
            m_pauseButton->setEnabled(true);
            m_stopButton->setEnabled(true);
            m_stepButton->setEnabled(false);
            m_speedDownButton->setEnabled(true);
            m_speedUpButton->setEnabled(true);
            break;
        case Paused:

            m_runButton->setChecked(false);

            m_runButton->setEnabled(true);
            m_pauseButton->setEnabled(false);
            m_stopButton->setEnabled(true);
            m_stepButton->setEnabled(true);
            m_speedDownButton->setEnabled(true);
            m_speedUpButton->setEnabled(true);
            break;
    }
}

void SimulationWidget::updateLabel() {
    m_cycleCounterLabel->setText("Cycles: " + QString::number(m_cycles));
}

void SimulationWidget::initSimulation() {
    // Инициализация симуляции
    // Создание объекта ffi симуляции сети
    auto parent = qobject_cast<GraphicsView*>(this->parent());
    auto scene = qobject_cast<GraphicScene*>(parent->scene());
    scene->setSimulation(true);

    m_simulation = ffi::Simulation::create(scene->net());

    m_cycles = 0;
    updateLabel();
}

void SimulationWidget::simulate() {
    // Выполнение одного цикла симуляции
    m_simulation->simulate();

    auto parent = qobject_cast<GraphicsView*>(this->parent());
    auto scene = qobject_cast<GraphicScene*>(parent->scene());
    // Установит маркеры позиций
    for (auto position : scene->positions()) {
        position->setMarkers(m_simulation->markers(position->vertexIndex()));
    }

    m_cycles++;
    updateLabel();

    scene->update();
}

void SimulationWidget::cancelSimulation() {
    // Очистка симуляции
    // scene
    auto parent = qobject_cast<GraphicsView*>(this->parent());
    auto scene = qobject_cast<GraphicScene*>(parent->scene());
    scene->setSimulation(false);

    // simulation
    m_simulation->destroy();
    m_simulation = nullptr;
    m_cycles = 0;
    updateLabel();

    scene->update();
}
