//
// Created by Николай Муравьев on 09.10.2022.
//

#include "MainTreeView.h"
#include "MainTreeModel.h"
#include "MainTreeItem.h"
#include <QMouseEvent>

MainTreeView::MainTreeView(MainTreeModel *model, QWidget *parent)
    : QTreeView(parent)
{
    setHeaderHidden(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    this->setModel(model);
}

void MainTreeView::mouseDoubleClickEvent(QMouseEvent *event) {
    auto index = this->indexAt(event->pos());
    qDebug() << "MainTreeView::mouseDoubleClickEvent(index = " << index << ")";

    if (index.isValid())
        emit elementAction(index);
}
