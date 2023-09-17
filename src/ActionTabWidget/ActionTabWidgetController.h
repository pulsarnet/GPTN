//
// Created by darkp on 16.09.2023.
//

#ifndef GPTN_ACTIONTABWIDGETCONTROLLER_H
#define GPTN_ACTIONTABWIDGETCONTROLLER_H

#include <QObject>

using namespace std;

class MainWindow;
class ActionTabWidget;

class ActionTabWidgetController : public QObject {

    Q_OBJECT

public:

    explicit ActionTabWidgetController(MainWindow*);

    bool openTab(const QString& name, const QIcon& icon, QWidget* widget);

private:

    MainWindow* m_mainWindow;
    ActionTabWidget* m_actionTabWidget;
};


#endif //GPTN_ACTIONTABWIDGETCONTROLLER_H
