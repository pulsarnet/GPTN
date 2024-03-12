/**
 *
 *
 */


#include "ActionTabWidget.h"
#include <QTabBar>
#include <QBoxLayout>

ActionTabWidget::ActionTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}