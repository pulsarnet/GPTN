//
// Created by darkp on 10.10.2022.
//

#ifndef FFI_RUST_REACHABILITYTREEITEM_H
#define FFI_RUST_REACHABILITYTREEITEM_H

#include "MainTreeItem.h"

class WrappedLayoutWidget;
class ProjectMetadata;

class ReachabilityTreeItem : public MainTreeItem {

    Q_OBJECT

public:

    explicit ReachabilityTreeItem(ProjectMetadata* metadata, MainTreeItem* parent);

    WrappedLayoutWidget* reachabilityTab();

    QVariant data(int column) const noexcept override;

    QIcon icon() const noexcept override;

private:

    WrappedLayoutWidget* m_reachabilityTab;
};


#endif //FFI_RUST_REACHABILITYTREEITEM_H
