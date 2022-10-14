//
// Created by darkp on 08.10.2022.
//

#include <QMenu>
#include "AnalysisTreeItem.h"
#include "ProjectTreeItem.h"
#include "DecomposeTreeItem.h"
#include "ReachabilityTreeItem.h"

AnalysisTreeItem::AnalysisTreeItem(ProjectTreeItem *parent)
    : MainTreeItem(parent)
    , m_decomposeTreeItem(nullptr)
    , m_reachabilityTreeItem(nullptr)
{
}

QVariant AnalysisTreeItem::data(int column) const noexcept {
    return "Analysis";
}

QMenu *AnalysisTreeItem::contextMenu() noexcept {
    auto menu = new QMenu;
    menu->deleteLater();

    if (!m_decomposeTreeItem) {
        auto decomposeAction = new QAction("Decompose", menu);
        connect(decomposeAction,
                &QAction::triggered,
                this,
                &AnalysisTreeItem::execDecomposeAction);

        menu->addAction(decomposeAction);
    }

    if (!m_reachabilityTreeItem) {
        auto reachabilityAction = new QAction("Reachability Tree", menu);
        connect(reachabilityAction,
                &QAction::triggered,
                this,
                &AnalysisTreeItem::execReachabilityAction);

        menu->addAction(reachabilityAction);
    }

    return menu;
}

void AnalysisTreeItem::execDecomposeAction(bool checked) {
    Q_UNUSED(checked)

    m_decomposeTreeItem = new DecomposeTreeItem(this);
    emit onChildAdd();
}

void AnalysisTreeItem::execReachabilityAction(bool checked) {
    Q_UNUSED(checked)

    m_reachabilityTreeItem = new ReachabilityTreeItem(this);
    emit onChildAdd();
}
