//
// Created by darkp on 16.09.2023.
//

#ifndef GPTN_ACTIONTABWIDGETCONTROLLER_H
#define GPTN_ACTIONTABWIDGETCONTROLLER_H

#include <QObject>
#include <QHash>

using namespace std;

class MainWindow;
class ActionTabWidget;

class ActionTabWidgetController : public QObject {

    Q_OBJECT

public:

    explicit ActionTabWidgetController(MainWindow*);

    bool openTab(const QString& name, const QIcon& icon, QWidget* widget);

public slots:

    void onTabChanged(int index);

private:

    MainWindow* m_mainWindow;
    ActionTabWidget* m_actionTabWidget;
    QHash<QString, int> m_openedTabs;
};


#endif //GPTN_ACTIONTABWIDGETCONTROLLER_H
