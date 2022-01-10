//
// Created by Николай Муравьев on 09.01.2022.
//

#include "../../include/synthesis/synthesis_view.h"
#include "../../include/ffi/rust.h"
#include "../../include/mainwindow.h"
#include "../../include/synthesis/synthesis_program_item_delegate.h"
#include <QHeaderView>

SynthesisView::SynthesisView(SynthesisProgram *program, Tab* tab, QWidget *parent) : QWidget(parent), m_tab(tab) {
    this->setWindowModality(Qt::WindowModal);

    m_add = new QPushButton("Add program");
    connect(m_add, &QPushButton::clicked, this, &SynthesisView::slotAddButton);

    m_confirm = new QPushButton("Confirm");
    connect(m_confirm, &QPushButton::clicked, this, &SynthesisView::slotConfirm);

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
    layout->addWidget(m_add);
    layout->addWidget(m_tableview);
    layout->addWidget(m_confirm);
}

void SynthesisView::slotAddButton(bool checked) {
    Q_UNUSED(checked);

    m_model->insertRow(m_model->rowCount(QModelIndex()));
}

void SynthesisView::slotConfirm(bool checked) {
    Q_UNUSED(checked);

    auto common_result = split_finish(m_model->program());

    dynamic_cast<MainWindow*>(m_tab->parent()->parent()->parent())->addTabFromNet(std::move(common_result),
                                                                                     m_tab);

}
