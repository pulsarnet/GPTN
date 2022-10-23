//
// Created by darkp on 08.10.2022.
//

#include "ProjectTreeItem.h"
#include "ModelTreeItem.h"
#include "AnalysisTreeItem.h"

ProjectTreeItem::ProjectTreeItem(const std::filesystem::path& path)
    : MainTreeItem(nullptr)
    , m_modelItem(new ModelTreeItem(this))
    , m_analysisItem(new AnalysisTreeItem(this))
{
    m_projectName = QString::fromStdString(path.filename().string()).left(m_projectName.lastIndexOf('.'));
    m_path = path;
}

QVariant ProjectTreeItem::data(int column) const noexcept {
    if (column != 0) return {};
    return m_projectName;
}
