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
#include <QAction>
#include <QToolButton>

DockToolbar::DockToolbar(ads::CDockAreaWidget *_parent) :
        ads::CDockAreaTitleBar(_parent), m_parent(nullptr), m_label(new QLabel("...", this))
{
    _parent->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setMinimumHeight(28);
    setMaximumHeight(30);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_label->setMinimumSize(QSize(0, 0));
    m_label->setMaximumSize(QSize(INT_MAX, INT_MAX));

    m_fullScreenButton = new QToolButton(this);
    m_fullScreenButton->setAutoRaise(true);
    m_fullScreenButton->setIcon(QIcon(":/images/fullscreen.svg"));
    m_fullScreenButton->setMinimumSize(QSize(24, 24));
    m_fullScreenButton->setMaximumSize(QSize(24, 24));
    m_fullScreenButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);

    insertWidget(indexOf(button(ads::TitleBarButtonTabsMenu)) - 1, m_label);
    insertWidget(indexOf(button(ads::TitleBarButtonClose)), m_fullScreenButton);

    connect(m_fullScreenButton, &QPushButton::clicked, this, &DockToolbar::signalFullScreen);
    connect(_parent, &ads::CDockAreaWidget::windowTitleChanged, this, &DockToolbar::onWindowTitleChanged);

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

