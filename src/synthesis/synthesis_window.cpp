//
// Created by darkp on 13.09.2022.
//

#include <QTableView>
#include <QItemSelectionModel>
#include <DockManager.h>
#include <DockAreaWidget.h>
#include <QGridLayout>
#include <QLabel>
#include "synthesis_window.h"
#include "synthesis_table.h"
#include "synthesis_model.h"
#include "../DockSystem/DockWidget.h"
#include "../Editor/GraphicsScene.h"
#include "../Editor/GraphicsView.h"
#include "../Core/FFI/rust.h"

SynthesisWindow::SynthesisWindow(ffi::DecomposeContext *ctx, QVector<size_t> programs, QWidget* parent)
    : QWidget(parent)
    , m_ctx(ctx)
    , m_manager(new ads::CDockManager(this))
{

    setMinimumSize(QSize(800, 600));

    auto table = new SynthesisTable(m_ctx, std::move(programs), this);
    connect(
            table->table()->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this,
            &SynthesisWindow::onSelectionChanged
            );

    // todo pass window
    auto view = new GraphicsView(nullptr, this);
    view->setToolBoxVisibility(false);

    m_table = new DockWidget("Table");
    m_table->setWidget(table);

    m_view = new DockWidget("View");
    m_view->setWidget(view);

    m_program = new DockWidget("Program");
    m_program->setWidget(new QLabel("Empty")); //TODO: добавить текст программы синтеза

    auto area = m_manager->addDockWidget(ads::RightDockWidgetArea, m_view);
    area->setWindowTitle("Графическое представление");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_manager->addDockWidget(ads::LeftDockWidgetArea, m_table, area);
    area->setWindowTitle("Программы");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    area = m_manager->addDockWidget(ads::BottomDockWidgetArea, m_program, area);
    area->setWindowTitle("Программа");
    area->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setLayout(new QGridLayout(this));
    layout()->addWidget(m_manager);
    layout()->setContentsMargins(0, 0, 0, 0);
}

void SynthesisWindow::onSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    Q_UNUSED(deselected)
    assert(selected.size() < 2);

    if (selected.empty()) {
        // clear view
        return;
    }

    QVariant programVariant = selected[0].model()->data(QModelIndex(selected.indexes()[0]));
    if (!programVariant.isValid()) {
        return;
    }

    int program = programVariant.toInt();
    auto result = m_ctx->eval_program(program);
    auto newScene = new GraphicsScene(result);
    auto oldScene = view()->scene();
    view()->setScene(newScene);
    delete oldScene;

    label()->setText(m_ctx->program_equations(program));
}

SynthesisTable *SynthesisWindow::table() {
    return qobject_cast<SynthesisTable*>(m_table->widget());
}

const SynthesisTable *SynthesisWindow::table() const {
    return qobject_cast<const SynthesisTable*>(m_table->widget());
}

GraphicsView *SynthesisWindow::view() {
    return qobject_cast<GraphicsView*>(m_view->widget());
}

const GraphicsView *SynthesisWindow::view() const {
    return qobject_cast<const GraphicsView*>(m_view->widget());
}

QLabel *SynthesisWindow::label() {
    return qobject_cast<QLabel*>(m_program->widget());
}

const QLabel *SynthesisWindow::label() const {
    return qobject_cast<const QLabel*>(m_program->widget());
}
