//
// Created by nmuravev on 1/20/2022.
//

#include "synthesis_program_item_delegate.h"
#include "column_delegate_type.h"

void ProgramItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto data = index.data(Qt::UserRole);
    if (!data.canConvert<ColumnDelegateType>()) return;

    auto type = qvariant_cast<ColumnDelegateType>(data);

    if (type == ColumnDelegateType::ShowButton) {
        QStyleOptionButton button;
        button.rect = option.rect;
        button.text = "=^.^=";
        button.state = QStyle::State_Enabled;

        QApplication::style()->drawControl( QStyle::CE_PushButton, &button, painter);
        return;
    }

    QItemDelegate::paint(painter, option, index);
}

bool ProgramItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                      const QModelIndex &index) {

    auto data = index.data(Qt::UserRole);
    if (event->type() == QEvent::MouseButtonRelease && data.canConvert<ColumnDelegateType>() && qvariant_cast<ColumnDelegateType>(data) == ShowButton) {
        auto e = (QMouseEvent*)event;
        auto pos = e->pos();

        if (option.rect.contains(pos.x(), pos.y())) {
            index.data(EvalProgramRole);
        }

        return true;
    }

    return QItemDelegate::editorEvent(event, model, option, index);
}

QWidget *ProgramItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    auto data = index.data(Qt::UserRole);
    if (data.canConvert<ColumnDelegateType>()) {
        auto type = qvariant_cast<ColumnDelegateType>(data);
        if (type == ColumnDelegateType::Value) {
            auto w = new QSpinBox(parent);
            connect(w, &QSpinBox::editingFinished, this, &ProgramItemDelegate::commitAndClose);
            return w;
        }
    }

    return nullptr;
}
