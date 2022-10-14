//
// Created by darkp on 08.10.2022.
//

#ifndef FFI_RUST_PROJECTTREEITEM_H
#define FFI_RUST_PROJECTTREEITEM_H

#include <filesystem>
#include "MainTreeItem.h"

class ModelTreeItem;
class AnalysisTreeItem;

class ProjectTreeItem : public MainTreeItem {

public:

    explicit ProjectTreeItem(const std::filesystem::path& path);

    ModelTreeItem* modelItem() { return m_modelItem; }
    AnalysisTreeItem* analysisItem() { return m_analysisItem; }

    QVariant data(int column) const noexcept override;

private:

    QString m_projectName;
    std::filesystem::path m_folder;

    ModelTreeItem* m_modelItem;
    AnalysisTreeItem* m_analysisItem;

};


#endif //FFI_RUST_PROJECTTREEITEM_H
