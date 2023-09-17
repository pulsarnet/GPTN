//
// Created by darkp on 16.09.2023.
//

#include "ActionTabWidgetController.h"
#include "../mainwindow.h"
#include "ActionTabWidget.h"

ActionTabWidgetController::ActionTabWidgetController(MainWindow *parent)
    : QObject(nullptr)
    , m_mainWindow(parent)
{
    m_actionTabWidget = new ActionTabWidget(m_mainWindow);
    connect(m_actionTabWidget, &ActionTabWidget::currentChanged, parent, &MainWindow::tabWidgetCurrentChanged);

    m_mainWindow->setCentralWidget(m_actionTabWidget);
}


bool ActionTabWidgetController::openTab(const QString &name, const QIcon &icon, QWidget *widget) {
    if (auto index = m_actionTabWidget->findTabContainsWidget(widget); index >= 0) {
        m_actionTabWidget->setCurrentIndex(index);
    } else {
        m_actionTabWidget->insertTab(
                m_actionTabWidget->currentIndex() >= 0 ? m_actionTabWidget->currentIndex() : 0,
                widget,
                icon,
                name
        );
    }
    return true;
}

