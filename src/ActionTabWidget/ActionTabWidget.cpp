/**
 *
 *
 */


#include "ActionTabWidget.h"
#include "NetModelingTab.h"
#include "DecomposeModelTab.h"
#include <QTabBar>
#include <QBoxLayout>
#include <QMenu>
#include <QLabel>

ActionTabWidget::ActionTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}