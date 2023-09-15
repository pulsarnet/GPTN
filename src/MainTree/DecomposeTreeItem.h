//
// Created by darkp on 10.10.2022.
//

#ifndef FFI_RUST_DECOMPOSETREEITEM_H
#define FFI_RUST_DECOMPOSETREEITEM_H

#include "MainTreeItem.h"

class ProjectMetadata;
class DecomposeModelTab;

class DecomposeTreeItem : public MainTreeItem {

public:

    explicit DecomposeTreeItem(ProjectMetadata* metadata, MainTreeItem* parent);

    DecomposeModelTab* decomposeModelTab();

    QVariant data(int column) const noexcept override;

    QIcon icon() const noexcept override;

private:

    DecomposeModelTab* m_decomposeTab;
};


#endif //FFI_RUST_DECOMPOSETREEITEM_H
