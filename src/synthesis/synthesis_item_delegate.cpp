//
// Created by nikolajmuravev on 31.01.2022.
//

#include "synthesis_item_delegate.h"
#include <QSpinBox>

QWidget *SynthesisItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                             const QModelIndex &index) const {
    auto box = new QSpinBox(parent);
    box->setMinimum(0);
    box->setButtonSymbols(QAbstractSpinBox::NoButtons);
    return box;
}

void SynthesisItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
    qobject_cast<QSpinBox*>(editor)->setValue(index.data().toInt());
    QItemDelegate::setEditorData(editor, index);
}

void SynthesisItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    model->setData(index, qobject_cast<QSpinBox*>(editor)->value());
    QItemDelegate::setModelData(editor, model, index);
}

SynthesisItemDelegate::SynthesisItemDelegate(QObject *parent) : QItemDelegate(parent) {

}
