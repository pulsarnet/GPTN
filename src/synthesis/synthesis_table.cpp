//
// Created by nmuravev on 23.01.2022.
//

#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>
#include <QHeaderView>
#include <QTableView>
#include "synthesis_model.h"
#include "synthesis_table.h"
#include "synthesis_item_delegate.h"

SynthesisTable::SynthesisTable(ffi::SynthesisContext *ctx, QWidget *parent): QWidget(parent), m_context(ctx) {
    setLayout(new QVBoxLayout(this));

    m_addProgram = new QAction("Add program");
    connect(m_addProgram, &QAction::triggered, this, &SynthesisTable::slotAddProgram);

    m_evalProgram = new QAction("Eval program");
    connect(m_evalProgram, &QAction::triggered, this, &SynthesisTable::slotEvalProgram);

    m_toolBar = new QToolBar(this);
    m_toolBar->addAction(m_addProgram);
    m_toolBar->addAction(m_evalProgram);

    m_table = new QTableView(this);
    m_table->setModel(new SynthesisModel(ctx));
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->setItemDelegate(new SynthesisItemDelegate);

    layout()->addWidget(m_toolBar);
    layout()->addWidget(m_table);
}

void SynthesisTable::slotEvalProgram(bool) {
    auto index = m_table->currentIndex();
    if (!index.isValid()) return;

    auto model = dynamic_cast<SynthesisModel*>(m_table->model());
    auto result = model->ctx()->eval_program(index.row());

    emit signalSynthesisedProgram(result);
}

void SynthesisTable::slotAddProgram(bool) {
    auto model = dynamic_cast<SynthesisModel*>(m_table->model());
    model->insertRow(model->rowCount(QModelIndex()));
}
