//
// Created by darkp on 16.09.2023.
//

#ifndef GPTN_APPLICATIONPROJECTCONTROLLER_H
#define GPTN_APPLICATIONPROJECTCONTROLLER_H

#include <QObject>
#include <vector>
#include <unordered_map>

class MainWindow;

class ApplicationProjectController : public QObject {

    Q_OBJECT

public:
    explicit ApplicationProjectController(QObject* parent = nullptr);

    MainWindow* createMainWindow(QWidget* parent = nullptr);

    bool openProject(const QString& filename, MainWindow* parent);
    void closeProject(const QString& filename);

private:
    std::vector<MainWindow*> mAvailableWindows;
    std::unordered_map<QString, MainWindow*> mOpenedProjects;
};


#endif //GPTN_APPLICATIONPROJECTCONTROLLER_H
