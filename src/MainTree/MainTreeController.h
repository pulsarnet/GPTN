//
// Created by darkp on 16.09.2023.
//

#ifndef GPTN_MAINTREECONTROLLER_H
#define GPTN_MAINTREECONTROLLER_H

#include <QObject>
#include <QModelIndex>

class MainWindow;
class MainTreeView;
class MainTreeModel;
class MainTreeItem;
class QDockWidget;

class MainTreeController : public QObject {
    Q_OBJECT

public:

    explicit MainTreeController(MainWindow* parent);

    bool addChildItem(MainTreeItem* item, const QModelIndex &parent = QModelIndex());
    MainTreeItem* getItem(const QModelIndex& index) const noexcept;

    QAction* toggleViewAction() const noexcept;

private:

    MainWindow* m_mainWindow;
    QDockWidget* m_mainTreeWidget;
    MainTreeView* m_mainTreeView;
    MainTreeModel* m_mainTreeModel;
};


#endif //GPTN_MAINTREECONTROLLER_H
