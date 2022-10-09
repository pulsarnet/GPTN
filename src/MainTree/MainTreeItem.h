//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_MAINTREEITEM_H
#define FFI_RUST_MAINTREEITEM_H

#include <QVariant>

class MainTreeItem {

public:

    explicit MainTreeItem(MainTreeItem* parent = nullptr);

    MainTreeItem* parentItem() noexcept { return m_parentItem; };
    MainTreeItem* childItem(int row) { return m_childItems[row]; }

    [[nodiscard]] virtual int childCount() const noexcept;
    [[nodiscard]] virtual int columnCount() const noexcept;
    [[nodiscard]] int childNumber() const noexcept;

    bool insertChild(int row, MainTreeItem* item);

    [[nodiscard]] virtual QVariant data(int column) const noexcept;
    [[nodiscard]] virtual QIcon icon() const noexcept;

    virtual ~MainTreeItem();

private:

    MainTreeItem* m_parentItem;
    QList<MainTreeItem*> m_childItems;

};


#endif //FFI_RUST_MAINTREEITEM_H
