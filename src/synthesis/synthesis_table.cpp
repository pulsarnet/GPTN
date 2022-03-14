//
// Created by nmuravev on 23.01.2022.
//

#include <QVBoxLayout>
#include <QAction>
#include <QToolBar>
#include <QHeaderView>
#include <QTableView>
#include <QSplitter>
#include "synthesis_model.h"
#include "synthesis_table.h"
#include "synthesis_item_delegate.h"
#include "../overrides/switch.h"
#include "../ffi/rust.h"

SynthesisTable::SynthesisTable(ffi::DecomposeContext *ctx, QWidget *parent): QWidget(parent), m_context(ctx) {
    setLayout(new QVBoxLayout(this));

    m_addProgram = new QAction("Add program");
    connect(m_addProgram, &QAction::triggered, this, &SynthesisTable::slotAddProgram);

    m_evalProgram = new QAction("Eval program");
    connect(m_evalProgram, &QAction::triggered, this, &SynthesisTable::slotEvalProgram);

    m_toolBar = new QToolBar(this);
    m_toolBar->addAction(m_addProgram);
    m_toolBar->addAction(m_evalProgram);

    auto model = new SynthesisModel(ctx);
    m_table = new QTableView(this);
    m_table->setModel(model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_table->setItemDelegate(new SynthesisItemDelegate);

    m_enableLabel = new Switch("Enable labels");
    connect(m_enableLabel, &QAbstractButton::toggled, model, &SynthesisModel::slotEnableLabels);
    m_toolBar->addWidget(new QSplitter(Qt::Horizontal));
    m_toolBar->addWidget(m_enableLabel);

    layout()->addWidget(m_toolBar);
    layout()->addWidget(m_table);
}

SynthesisModel *SynthesisTable::model() const {
    return dynamic_cast<SynthesisModel*>(m_table->model());
}

void SynthesisTable::slotEvalProgram(bool) {
    auto index = m_table->currentIndex();
    if (!index.isValid()) return;

    auto model = dynamic_cast<SynthesisModel*>(m_table->model());
    auto result = model->ctx()->eval_program(index.row());

    emit signalSynthesisedProgram(result, index.row());
}

void SynthesisTable::slotAddProgram(bool) {
    auto model = dynamic_cast<SynthesisModel*>(m_table->model());
    model->insertRow(model->rowCount(QModelIndex()));
}
