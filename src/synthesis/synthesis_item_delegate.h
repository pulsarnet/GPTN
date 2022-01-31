//
// Created by nikolajmuravev on 31.01.2022.
//

#ifndef FFI_RUST_SYNTHESIS_ITEM_DELEGATE_H
#define FFI_RUST_SYNTHESIS_ITEM_DELEGATE_H

#include <QItemDelegate>

class SynthesisItemDelegate : public QItemDelegate {

public:

    explicit SynthesisItemDelegate(QObject* parent = nullptr);

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;

private:

};


#endif //FFI_RUST_SYNTHESIS_ITEM_DELEGATE_H
