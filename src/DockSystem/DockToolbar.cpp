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

DockToolbar::DockToolbar(ads::CDockAreaWidget *parent) :
        ads::CDockAreaTitleBar(parent), m_parent(nullptr), m_label(new QLabel("...", this))
{
    parent->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

    setMinimumHeight(28);
    setMaximumHeight(30);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    auto spacer1 = new QSpacerItem(40, 20, QSizePolicy::Fixed, QSizePolicy::Fixed);
    auto spacer2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_label->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    m_label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_fullScreenButton = new QPushButton;
    m_fullScreenButton->setIcon(QIcon(":/images/fullscreen.svg"));
    m_fullScreenButton->setMinimumSize(QSize(24, 24));
    m_fullScreenButton->setMaximumSize(QSize(24, 24));
    m_fullScreenButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    insertWidget(0, m_label);
    insertWidget(1, m_fullScreenButton);

    tabBar()->setVisible(false);
    tabBar()->setEnabled(false);
    connect(m_fullScreenButton, &QPushButton::clicked, this, &DockToolbar::signalFullScreen);

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

