//
// Created by Николай Муравьев on 09.10.2022.
//

#include "MainTreeView.h"
#include "MainTreeModel.h"
#include <QMouseEvent>

MainTreeView::MainTreeView(MainTreeModel *model, QWidget *parent)
    : QTreeView(parent)
{
    setHeaderHidden(true);

    this->setModel(model);
}

void MainTreeView::mouseDoubleClickEvent(QMouseEvent *event) {
    auto index = this->indexAt(event->pos());

    if (index.isValid())
        emit elementAction(index);
}
