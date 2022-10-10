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

void ActionTabWidget::slotCloseTab(int index) {
    if (index == -1)
        return;

    removeTab(index);
}