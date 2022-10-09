//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_ANALYSISTREEITEM_H
#define FFI_RUST_ANALYSISTREEITEM_H

#include "MainTreeItem.h"

class ModelTreeItem;

class AnalysisTreeItem : public MainTreeItem {

public:

    explicit AnalysisTreeItem(ModelTreeItem* parent);

    QVariant data(int column) const noexcept override;
};


#endif //FFI_RUST_ANALYSISTREEITEM_H
