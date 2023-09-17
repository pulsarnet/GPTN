//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_MAINTREEITEM_H
#define FFI_RUST_MAINTREEITEM_H

#include <QVariant>
#include <QIcon>

class QMenu;

class MainTreeItem : public QObject{

    Q_OBJECT

public:

    explicit MainTreeItem(QString name, QIcon icon = QIcon(), MainTreeItem* parent = nullptr);
    explicit MainTreeItem(QString name, QIcon icon, QWidget* widget, MainTreeItem* parent = nullptr);

    MainTreeItem* parentItem() noexcept { return m_parentItem; };
    MainTreeItem* childItem(int row) { return m_childItems[row]; }

    [[nodiscard]] virtual int childCount() const noexcept;
    [[nodiscard]] virtual int columnCount() const noexcept;
    [[nodiscard]] int childNumber() const noexcept;

    bool insertChild(int row, MainTreeItem* item);
    bool removeChildren(int position, int count) noexcept;

    [[nodiscard]] virtual QVariant data(int column) const noexcept;
    [[nodiscard]] virtual QIcon icon() const noexcept;
    [[nodiscard]] virtual QWidget* widget() const noexcept;

    virtual ~MainTreeItem();

signals:

    void onChildAdd();

private:

    MainTreeItem* m_parentItem;
    QList<MainTreeItem*> m_childItems;

    QString m_name;
    QIcon m_icon;
    QWidget* m_widget;
};


#endif //FFI_RUST_MAINTREEITEM_H
