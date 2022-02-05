#ifndef TREEVIEW_H
#define TREEVIEW_H

#include <QTreeView>

class TreeView : public QTreeView
{
public:
    explicit TreeView(QWidget* parent = nullptr);

public slots:

    void onCustomMenu(const QPoint &point);
    void onDoubleClick(const QModelIndex& index);

private:


};

#endif // TREEVIEW_H
