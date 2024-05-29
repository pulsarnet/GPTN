#ifndef GPTN_ACTIONTABWIDGETCONTROLLER_H
#define GPTN_ACTIONTABWIDGETCONTROLLER_H

#include <QObject>

using namespace std;

class MainWindow;
class ActionTabWidget;
class QMenu;

class ActionTabWidgetController : public QObject {

    Q_OBJECT

public:

    explicit ActionTabWidgetController(MainWindow*);

    int addTab(const QString& name, const QIcon& icon, QWidget* widget);
    int indexOf(QWidget* widget) const;
    void setTabCloseable(int index, bool closeable = true);
    QWidget* widget(int index) const;
    void setCurrentIndex(int index);

public slots:

    void onTabChanged(int index);
    void onTabCloseRequested(int index);

private:

    MainWindow* m_mainWindow;
    ActionTabWidget* m_actionTabWidget;
};


#endif //GPTN_ACTIONTABWIDGETCONTROLLER_H
