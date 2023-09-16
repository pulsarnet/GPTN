#include "../mainwindow.h"
#include "MainTreeController.h"
#include "MainTreeModel.h"
#include "MainTreeView.h"
#include <QDockWidget>

MainTreeController::MainTreeController(MainWindow *parent)
    : QObject(nullptr)
    , m_mainWindow(parent)
{
    m_mainTreeWidget = new QDockWidget(tr("Project Tree"), (QWidget*)m_mainWindow);
    m_mainTreeWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_mainTreeWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    m_mainTreeModel = new MainTreeModel();
    m_mainTreeView = new MainTreeView(m_mainTreeModel, (QWidget*)m_mainWindow);
    m_mainTreeWidget->setWidget(m_mainTreeView);

    connect(m_mainTreeView, &MainTreeView::elementAction, m_mainWindow, &MainWindow::treeItemAction);
    connect(m_mainTreeView, &MainTreeView::customContextMenuRequested, m_mainWindow, &MainWindow::treeItemContextMenuRequested);

    m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_mainTreeWidget);
}

bool MainTreeController::addChildItem(MainTreeItem *item, const QModelIndex &parent) {
    Q_ASSERT(item);
    return m_mainTreeModel->addChild(item, parent);
}

QAction *MainTreeController::toggleViewAction() const noexcept {
    return m_mainTreeWidget->toggleViewAction();
}

MainTreeItem *MainTreeController::getItem(const QModelIndex &index) const noexcept {
    return m_mainTreeModel->getItem(index);
}