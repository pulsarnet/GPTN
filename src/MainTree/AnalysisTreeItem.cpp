//
// Created by darkp on 08.10.2022.
//

#include "AnalysisTreeItem.h"
#include "ModelTreeItem.h"

AnalysisTreeItem::AnalysisTreeItem(ModelTreeItem *parent)
    : MainTreeItem(parent)
{
}

QVariant AnalysisTreeItem::data(int column) const noexcept {
    return "Analysis";
}
