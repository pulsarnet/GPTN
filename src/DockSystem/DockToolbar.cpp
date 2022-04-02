//
// Created by nmuravev on 3/21/2022.
//

#include "DockToolbar.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QInputDialog>
#include <DockAreaWidget.h>
#include <DockAreaTitleBar.h>
#include <DockAreaTabBar.h>
#include <DockManager.h>
#include <QAction>
#include <QToolButton>

QIcon DockToolbar::minimizeIcon() {
    static QIcon icon = QIcon(":/images/minimize.svg");
    return icon;
}

QIcon DockToolbar::expandIcon() {
    static QIcon icon = QIcon(":/images/fullscreen.svg");
    return icon;
}

DockToolbar::DockToolbar(ads::CDockAreaWidget *_parent) :
        ads::CDockAreaTitleBar(_parent), m_parent(nullptr), m_label(new QLabel("..."))
{
    _parent->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setMinimumHeight(28);
    setMaximumHeight(30);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_label->setContentsMargins(0, 0, 0, 0);

    m_fullScreenButton = new QToolButton(this);
    m_fullScreenButton->setAutoRaise(true);
    m_fullScreenButton->setIcon(DockToolbar::expandIcon());
    m_fullScreenButton->setMinimumSize(QSize(24, 24));
    m_fullScreenButton->setMaximumSize(QSize(24, 24));
    m_fullScreenButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    insertWidget(indexOf(button(ads::TitleBarButtonTabsMenu)) - 1, new SpacerWidget);
    insertWidget(indexOf(button(ads::TitleBarButtonTabsMenu)) - 1, m_label);
    insertWidget(indexOf(button(ads::TitleBarButtonClose)), m_fullScreenButton);

    connect(m_fullScreenButton, &QPushButton::clicked, this, &DockToolbar::toggleFullScreen);
    connect(_parent, &ads::CDockAreaWidget::windowTitleChanged, this, &DockToolbar::onWindowTitleChanged);

    button(ads::TitleBarButtonClose)->setHidden(true);

    m_label->installEventFilter(this);

}

bool DockToolbar::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseButtonDblClick) {
        bool ok = true;
        QString newName =
                QInputDialog::getText(this,
                                      tr("Change Name"),
                                      tr("Enter name:"),
                                      QLineEdit::Normal,
                                      m_label->text(),
                                      &ok);

        if (ok) {
            m_label->setText(newName);
        }
        return true;
    }

    return QObject::eventFilter(object, event);
}

void DockToolbar::onWindowTitleChanged(const QString &title) {
    m_label->setText(title);
}

void DockToolbar::toggleFullScreen() {
    if (m_fullScreen) {
        m_fullScreenButton->setIcon(DockToolbar::expandIcon());
        m_fullScreen = false;
    }
    else {
        m_fullScreenButton->setIcon(DockToolbar::minimizeIcon());
        m_fullScreen = true;
    }

    auto areaWidget = qobject_cast<ads::CDockAreaWidget*>(parent());
    auto dockManager = areaWidget->dockManager();
    for (int i = 0; i < dockManager->dockAreaCount(); i++)
    {
        auto area = dockManager->dockArea(i);
        if (area != areaWidget)
        {
            area->setVisible(!isFullScreen());
        }
    }
}

bool DockToolbar::isFullScreen() const {
    return m_fullScreen;
}

SpacerWidget::SpacerWidget(QWidget *parent) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setStyleSheet("border: none; background: none;");
}