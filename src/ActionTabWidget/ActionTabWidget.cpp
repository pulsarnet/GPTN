//
// Created by nmuravev on 3/20/2022.
//

#include "ActionTabWidget.h"
#include "NetModelingTab.h"
#include "DecomposeModelTab.h"
#include <QTabBar>
#include <QBoxLayout>
#include <QMenu>
#include <QLabel>

ActionTabWidget::ActionTabWidget(QWidget *parent) : QTabWidget(parent) {

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_decompose = new QAction("Декомпозиция", this);
    m_ddr = new QAction("Дерево достижимости", this);
    m_SLAE = new QAction("Решение СЛАУ");

    connect(m_decompose, &QAction::triggered, this, &ActionTabWidget::slotDecompose);
    connect(m_ddr, &QAction::triggered, this, &ActionTabWidget::slotDDR);
    connect(m_SLAE, &QAction::triggered, this, &ActionTabWidget::slotSLAE);

    m_newTabMenu = new QMenu(this);
    m_newTabMenu->addAction(m_decompose);
    m_newTabMenu->addAction(m_ddr);
    m_newTabMenu->addAction(m_SLAE);

    auto added = tabBar()->addTab("+");
    setTabToolTip(added, "Add a new tab to the current workspace");

    m_netModelingTab = new NetModelingTab;
    auto tab = insertTab(0, m_netModelingTab, "Моделирование");
    setTabIcon(0, QIcon(":/images/modeling.svg"));
    setCurrentIndex(tab);

    connect(this, &QTabWidget::tabBarClicked, this, &ActionTabWidget::slotTabBarClicked);

}

QVariant ActionTabWidget::saveState() const {
    QVariantHash result;

    QVariant netModelingTab;
    m_netModelingTab->saveState(netModelingTab);
    result["mainNetData"] = std::move(netModelingTab);

    return result;
}

void ActionTabWidget::restoreState(const QVariant &data) {
    QVariantHash hash = data.toHash();
    m_netModelingTab->restoreState(hash["mainNetData"]);
}

void ActionTabWidget::slotTabBarClicked(int index) {
    if (index == -1) return;

    if (index == (tabBar()->count() - 1)) {
        m_newTabMenu->exec(mapToGlobal(tabBar()->tabRect(index).bottomLeft()));
    }
    else
        setCurrentIndex(index);
}

void ActionTabWidget::slotDecompose() {

    // Сначала создадим декомпозицию
    m_netModelingTab->ctx()->decompose();

    m_decomposeModelTab = new DecomposeModelTab(m_netModelingTab, this);
    auto index = insertTab(tabBar()->count() - 1, m_decomposeModelTab, "Декомпозиция и синтез");
    setTabIcon(index, QIcon(":/images/decompose.svg"));
}

void ActionTabWidget::slotDDR() {}

void ActionTabWidget::slotSLAE() {}