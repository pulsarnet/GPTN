//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_ITEM_DELEGATE_H
#define FFI_RUST_SYNTHESIS_PROGRAM_ITEM_DELEGATE_H

#include <QItemDelegate>
#include <QSpinBox>

class ProgramItemDelegate : public QItemDelegate {

public:

    QWidget *
    createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        auto data = index.data(Qt::EditRole);
        auto w = new QSpinBox(parent);
        connect(w, &QSpinBox::editingFinished, this, &ProgramItemDelegate::commitAndClose);
        return w;
    }

public slots:

    void commitAndClose() {
        QSpinBox *editor = qobject_cast<QSpinBox *>(sender());
        emit commitData(editor);
        emit closeEditor(editor);
    };


};

#endif //FFI_RUST_SYNTHESIS_PROGRAM_ITEM_DELEGATE_H
