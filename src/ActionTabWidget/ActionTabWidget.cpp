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
    setTabsClosable(true);

    connect(this,
            &QTabWidget::tabCloseRequested,
            this,
            &ActionTabWidget::slotCloseTab);

}

/**
 * Возвращает индекс вкладки, адрес которой соответствует widget
 *
 * @param widget виджет, который необходимо найти
 * @return -1 - если не найдено, >= 0 - если найдено
 */
int ActionTabWidget::findTabContainsWidget(QWidget *widget) noexcept {
    for (int i = 0; i < this->count(); i++) {
        auto it = this->widget(i);
        if (it == widget) {
            return i;
        }
    }
    return -1;
}

void ActionTabWidget::slotCloseTab(int index) {
    if (index == -1)
        return;

    removeTab(index);
}