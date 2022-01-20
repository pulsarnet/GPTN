//
// Created by Николай Муравьев on 09.01.2022.
//

#include "synthesis_view.h"
#include "synthesis_programs_model.h"
#include "../tab.h"
#include "../mainwindow.h"
#include "synthesis_program_item_delegate.h"
#include <QHeaderView>

SynthesisView::SynthesisView(SynthesisProgram *program, Tab* tab, QWidget *parent) : QWidget(parent), m_tab(tab) {
    auto toolBar = new QToolBar;
    m_add = new QAction("Add program");
    connect(m_add, &QAction::triggered, this, &SynthesisView::slotAddButton);

    m_confirm = new QAction("Confirm");
    connect(m_confirm, &QAction::triggered, this, &SynthesisView::slotConfirm);

    toolBar->addAction(m_add);
    toolBar->addAction(m_confirm);

    m_tableview = new QTableView;

    m_tableview->setItemDelegate(new ProgramItemDelegate);

    QHeaderView* vert = m_tableview->verticalHeader();
    vert->setSectionResizeMode(QHeaderView::Fixed);
    vert->setDefaultSectionSize(30);

    QHeaderView* horz = m_tableview->horizontalHeader();
    horz->setSectionResizeMode(QHeaderView::Fixed);
    horz->setDefaultSectionSize(30);
    m_model = new SynthesisProgramModel(program);

    m_tableview->setModel(m_model);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(toolBar);
    layout->addWidget(m_tableview);
}

void SynthesisView::slotAddButton(bool checked) {
    Q_UNUSED(checked);

    m_model->insertRow(m_model->rowCount(QModelIndex()));
}

void SynthesisView::slotConfirm(bool checked) {
    Q_UNUSED(checked);
}
