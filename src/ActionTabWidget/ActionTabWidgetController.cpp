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
    m_actionTabWidget->setTabsClosable(true);

    connect(m_actionTabWidget, &QTabWidget::currentChanged, m_mainWindow, &MainWindow::onTabChanged);
    connect(m_actionTabWidget, &QTabWidget::tabCloseRequested, this, &ActionTabWidgetController::onTabCloseRequested);

    m_mainWindow->setCentralWidget(m_actionTabWidget);
}

/**
 * Добавляет вкладку
 *
 * @param name - имя вкладки, должно быть уникальным
 * @param icon - иконка вкладки
 * @param widget - виджет отображаемый во вкладке
 *
 * @return индекс добавленной вкладки, иначе -1
 */
int ActionTabWidgetController::addTab(const QString &name, const QIcon &icon, QWidget *widget) {
    Q_ASSERT(widget);
    int index = m_actionTabWidget->indexOf(widget);
    if (index >= 0) {
        return -1;
    }

    int tabIdx = m_actionTabWidget->insertTab(
            m_actionTabWidget->currentIndex() >= 0 ? m_actionTabWidget->currentIndex() + 1 : 0,
            widget,
            icon,
            name
    );
    m_actionTabWidget->setCurrentIndex(tabIdx);
    return tabIdx;
}

int ActionTabWidgetController::indexOf(QWidget* widget) const {
    return m_actionTabWidget->indexOf(widget);
}

void ActionTabWidgetController::setTabCloseable(int index, bool closeable) {
    Q_ASSERT(index >= 0);
    if (auto btn = m_actionTabWidget->tabBar()->tabButton(index, QTabBar::LeftSide); btn) {
        btn->setVisible(closeable);
    }

    if (auto btn = m_actionTabWidget->tabBar()->tabButton(index, QTabBar::RightSide); btn) {
        btn->setVisible(closeable);
    }
}

QWidget *ActionTabWidgetController::widget(int index) const {
    Q_ASSERT(index >= 0);
    return m_actionTabWidget->widget(index);
}

void ActionTabWidgetController::setCurrentIndex(int index) {
    Q_ASSERT(index >= 0 && index < m_actionTabWidget->count());
    m_actionTabWidget->setCurrentIndex(index);
}

void ActionTabWidgetController::onTabChanged(int index) {
    if (index == -1) {
        return;
    }
}

void ActionTabWidgetController::onTabCloseRequested(int index) {
    Q_ASSERT(index >= 0);
    m_actionTabWidget->removeTab(index);
}
