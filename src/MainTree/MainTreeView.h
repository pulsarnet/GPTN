//
// Created by Николай Муравьев on 09.10.2022.
//

#ifndef FFI_RUST_MAINTREEVIEW_H
#define FFI_RUST_MAINTREEVIEW_H

#include <QTreeView>

class MainTreeModel;

class MainTreeView : public QTreeView {

    Q_OBJECT

public:

    explicit MainTreeView(MainTreeModel* model, QWidget* parent = nullptr);

    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:

    void elementAction(const QModelIndex& index);

};


#endif //FFI_RUST_MAINTREEVIEW_H
