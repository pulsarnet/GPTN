//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_ANALYSISTREEITEM_H
#define FFI_RUST_ANALYSISTREEITEM_H

#include "MainTreeItem.h"

class ModelTreeItem;
class DecomposeTreeItem;
class ReachabilityTreeItem;

class AnalysisTreeItem : public MainTreeItem {

    Q_OBJECT

public:

    explicit AnalysisTreeItem(ModelTreeItem* parent);

    QVariant data(int column) const noexcept override;

    QMenu * contextMenu() noexcept override;

public slots:

    void execDecomposeAction(bool);

    void execReachabilityAction(bool);

private:

    DecomposeTreeItem* m_decomposeTreeItem;
    ReachabilityTreeItem* m_reachabilityTreeItem;
};


#endif //FFI_RUST_ANALYSISTREEITEM_H
