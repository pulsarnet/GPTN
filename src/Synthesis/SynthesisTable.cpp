//
// Created by nmuravev on 23.01.2022.
//

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableView>
#include "SynthesisModel.h"
#include "SynthesisTable.h"
#include "../Core/FFI/rust.h"

SynthesisTable::SynthesisTable(ffi::DecomposeContext *ctx, QVector<size_t> programs, QWidget *parent)
    : QWidget(parent)
{
    setLayout(new QVBoxLayout(this));

    auto model = new SynthesisModel(ctx, std::move(programs));
    m_table = new QTableView(this);
    m_table->setModel(model);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->verticalHeader()->hide();
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    layout()->addWidget(m_table);
}

SynthesisModel *SynthesisTable::model() const {
    return dynamic_cast<SynthesisModel*>(m_table->model());
}

QTableView *SynthesisTable::table() {
    return this->m_table;
}

const QTableView *SynthesisTable::table() const {
    return this->m_table;
}