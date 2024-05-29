#include "BaseTab.h"

BaseTab::BaseTab(QWidget* parent): QWidget(parent) {}

void BaseTab::addViewAction(QAction* action) {
    m_viewActions.push_back(action);
}

void BaseTab::removeViewAction(QAction* action) {
    m_viewActions.erase(
        std::remove(m_viewActions.begin(), m_viewActions.end(), action),
        m_viewActions.end()
    );
}

const QList<QAction *>& BaseTab::viewActions() const {
    return m_viewActions;
}


