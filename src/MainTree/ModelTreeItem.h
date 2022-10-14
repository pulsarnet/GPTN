//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_MODELTREEITEM_H
#define FFI_RUST_MODELTREEITEM_H

#include "ProjectTreeItem.h"
#include "../ActionTabWidget/NetModelingTab.h"


class ModelTreeItem : public MainTreeItem {

public:

    explicit ModelTreeItem(ProjectTreeItem* parent);

    QVariant data(int column) const noexcept override;
    QIcon icon() const noexcept override;

    NetModelingTab* netModelingTab();

private:

    NetModelingTab* m_modelingTab;
};


#endif //FFI_RUST_MODELTREEITEM_H
