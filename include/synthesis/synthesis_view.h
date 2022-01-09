//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_VIEW_H
#define FFI_RUST_SYNTHESIS_VIEW_H

#include <QWidget>
#include <QTableView>
#include <QPushButton>
#include <QVBoxLayout>
#include "synthesis_programs_model.h"
#include "../tab.h"

class SynthesisView : public QWidget {

    Q_OBJECT

public:
    explicit SynthesisView(SynthesisProgram* program, Tab* tab, QWidget* parent = nullptr);

public slots:

    void slotAddButton(bool checked);;

    void slotConfirm(bool checked);

private:

    QTableView* m_tableview = nullptr;
    SynthesisProgramModel* m_model = nullptr;
    QPushButton* m_add = nullptr;
    QPushButton* m_confirm = nullptr;
    Tab* m_tab = nullptr;

};

#endif //FFI_RUST_SYNTHESIS_VIEW_H
