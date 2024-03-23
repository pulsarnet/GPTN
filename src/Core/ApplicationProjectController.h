#ifndef GPTN_APPLICATIONPROJECTCONTROLLER_H
#define GPTN_APPLICATIONPROJECTCONTROLLER_H

#include <QObject>
#include <unordered_map>

class MainWindow;

class ApplicationProjectController : public QObject {

    Q_OBJECT

public:
    explicit ApplicationProjectController(QObject* parent = nullptr);

    MainWindow* createMainWindow(QWidget* parent = nullptr);

    void emptyProject();
    bool openProject(const QString& filename, MainWindow* parent);
    void closeProject(const QString& filename);

    const std::unordered_map<QString, MainWindow*>& openedProjects() const;

private:

    std::unordered_map<QString, MainWindow*> mOpenedProjects;
};


#endif //GPTN_APPLICATIONPROJECTCONTROLLER_H
