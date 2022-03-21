//
// Created by nmuravev on 3/20/2022.
//

#include "WrappedLayoutWidget.h"
#include <QBoxLayout>

WrappedLayoutWidget::WrappedLayoutWidget(QWidget *inner, QWidget* parent) : QWidget(parent), m_inner(inner) {
    setLayout(new QBoxLayout(QBoxLayout::TopToBottom));
    layout()->addWidget(inner);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(0);
}