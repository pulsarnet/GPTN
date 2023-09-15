#ifndef FFI_RUST_MODELTREEITEM_H
#define FFI_RUST_MODELTREEITEM_H

#include "../ActionTabWidget/NetModelingTab.h"
#include "MainTreeItem.h"

class ModelTreeItem : public MainTreeItem {

public:

    explicit ModelTreeItem(MainTreeItem* item, NetModelingTab* tab);

    QVariant data(int column) const noexcept override;
    QIcon icon() const noexcept override;

    NetModelingTab* netModelingTab();

private:

    NetModelingTab* m_modelingTab;
};


#endif //FFI_RUST_MODELTREEITEM_H
