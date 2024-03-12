#ifndef BASETAB_H
#define BASETAB_H

#include <QWidget>

class BaseTab: public QWidget {

public:

    explicit BaseTab(QWidget* parent = nullptr);

    const QList<QAction*>& viewActions() const;

protected:

    void addViewAction(QAction* action);

    void removeViewAction(QAction* action);

private:

    QList<QAction*> m_viewActions;
};

#endif //BASETAB_H
