//
// Created by nmuravev on 3/21/2022.
//

#include <QBoxLayout>
#include "DockWidget.h"
#include <DockAreaWidget.h>
#include <QAction>
#include <DockManager.h>
#include <DockAreaTitleBar.h>

QIcon minimizeIcon();
QIcon expandIcon();

DockWidget::DockWidget(const QString& name, QWidget *parent) : ads::CDockWidget(name, parent) {
    setFrameShape(QFrame::NoFrame);

    setFeature(ads::CDockWidget::DockWidgetFloatable, false);
    setFeature(ads::CDockWidget::NoTab, true);
    setFeature(ads::CDockWidget::DockWidgetClosable, false);
    setFeature(ads::CDockWidget::DockWidgetFocusable, true);

    layout()->setContentsMargins(0, 0, 0, 0);

    m_fullscreenAction = new QAction(expandIcon(), "Fullscreen");
    connect(m_fullscreenAction, &QAction::triggered, this, &DockWidget::onFullscreen);

    setTitleBarActions(QList { m_fullscreenAction });
}

void DockWidget::onFullscreen() {
    if (m_fullscreen) {
        m_fullscreenAction->setIcon(expandIcon());
        m_fullscreen = false;
    } else {
        m_fullscreenAction->setIcon(minimizeIcon());
        m_fullscreen = true;
    }

    auto dockManager = this->dockManager();
    for (int i = 0; i < dockManager->dockAreaCount(); i++) {
        auto area = dockManager->dockArea(i);
        if (area != this->dockAreaWidget()) {
            area->setVisible(!m_fullscreen);
        }
    }
}

QIcon minimizeIcon() {
    static QIcon icon = QIcon(":/images/minimize.svg");
    return icon;
}

QIcon expandIcon() {
    static QIcon icon = QIcon(":/images/fullscreen.svg");
    return icon;
}