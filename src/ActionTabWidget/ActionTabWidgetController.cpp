#include "ActionTabWidgetController.h"
#include "../MainWindow.h"
#include "ActionTabWidget.h"
#include "NetModelingTab.h"
#include "../Editor/GraphicsScene.h"
#include "../Editor/GraphicsSceneActions.h"

ActionTabWidgetController::ActionTabWidgetController(MainWindow *parent)
    : QObject(nullptr)
    , m_mainWindow(parent)
{
    m_actionTabWidget = new ActionTabWidget(m_mainWindow);
    m_mainWindow->setCentralWidget(m_actionTabWidget);
}

bool ActionTabWidgetController::openTab(const QString &name, const QIcon &icon, QWidget *widget) {
    if (auto index = m_actionTabWidget->findTabContainsWidget(widget); index >= 0) {
        m_actionTabWidget->setCurrentIndex(index);
    } else {
        int pos = m_actionTabWidget->insertTab(
                m_actionTabWidget->currentIndex() >= 0 ? m_actionTabWidget->currentIndex() + 1 : 0,
                widget,
                icon,
                name
        );
        m_actionTabWidget->setCurrentIndex(pos);
    }
    return true;
}

void ActionTabWidgetController::onTabChanged(int index) {
    if (index == -1) {
        return;
    }

}
