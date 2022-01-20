//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_PROGRAM_ITEM_DELEGATE_H
#define FFI_RUST_SYNTHESIS_PROGRAM_ITEM_DELEGATE_H

#include <QItemDelegate>
#include <QSpinBox>
#include <QApplication>
#include <QDialog>
#include <QMouseEvent>

class ProgramItemDelegate : public QItemDelegate {

public:

    enum {EvalProgramRole = Qt::UserRole + 1};

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

public slots:

    void commitAndClose() {
        QSpinBox *editor = qobject_cast<QSpinBox *>(sender());
        emit commitData(editor);
        emit closeEditor(editor);
    }


};

#endif //FFI_RUST_SYNTHESIS_PROGRAM_ITEM_DELEGATE_H
