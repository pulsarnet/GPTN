#include "treeview.h"
#include "treeitem.h"
#include "treemodel.h"
#include <QMenu>

TreeView::TreeView(QWidget* parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    setUpdatesEnabled(true);
    setSelectionMode(QTreeView::SingleSelection);
    connect(this, &QTreeView::customContextMenuRequested, this, &TreeView::onCustomMenu);
    connect(this, &QTreeView::doubleClicked, this, &TreeView::onDoubleClick);

    setExpandsOnDoubleClick(false);
}

void TreeView::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Delete) {
        const QModelIndex index = selectionModel()->currentIndex();
        if (index.isValid())
            model()->removeRow(index.row(), index.parent());
    }
}

void TreeView::onCustomMenu(const QPoint &point)
{
    QModelIndex index = this->indexAt(point);
    QMenu* menu = nullptr;
    if (index.isValid()) {
        menu = static_cast<TreeItem*>(index.internalPointer())->contextMenu();
    }
    else {
        menu = static_cast<TreeModel*>(model())->root()->contextMenu();
    }

    if (menu) {
        menu->exec(viewport()->mapToGlobal(point));
    }
}

void TreeView::onDoubleClick(const QModelIndex &index) {
    auto dock = static_cast<TreeItem*>(index.internalPointer())->dockWidget();
    if (dock) {
        dock->toggleView();
    }
}
