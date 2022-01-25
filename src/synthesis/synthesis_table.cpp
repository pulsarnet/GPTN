//
// Created by nmuravev on 23.01.2022.
//

#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>
#include "synthesis_model.h"
#include "synthesis_table.h"

SynthesisTable::SynthesisTable(ffi::SynthesisContext *ctx, QWidget *parent): QWidget(parent), m_context(ctx) {
    setLayout(new QVBoxLayout(this));

    auto evalProgram = new QAction("Eval program");
    connect(evalProgram, &QAction::triggered, this, &SynthesisTable::slotEvalProgram);

    auto tools = new QToolBar(this);
    tools->addAction(evalProgram);

    m_table = new QTableView(this);
    m_table->setModel(new SynthesisModel(ctx));

    layout()->addWidget(tools);
    layout()->addWidget(m_table);
}

void SynthesisTable::slotEvalProgram(bool) {
    auto index = m_table->currentIndex();
    if (!index.isValid()) return;

    auto model = dynamic_cast<SynthesisModel*>(m_table->model());
    auto result = model->ctx()->eval_program(index.row());

    emit signalSynthesisedProgram(result);
}
