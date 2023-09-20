//
// Created by darkp on 16.09.2023.
//

#include "ApplicationProjectController.h"
#include "../MainWindow.h"
#include <QApplication>

ApplicationProjectController::ApplicationProjectController(QObject *parent)
    : QObject(parent)
{

}

MainWindow *ApplicationProjectController::createMainWindow(QWidget* parent) {
    auto window = new MainWindow(this, parent);
    window->setMinimumSize(QSize(1280, 720));
    mAvailableWindows.push_back(window);
    return window;
}

/**
 * Открывает проект в новом или свободном окне
 *
 * @param filename - абсолютный путь до файла проекта
 * @return true - если это успех, false - не удалось открыть проект
 */
bool ApplicationProjectController::openProject(const QString &filename, MainWindow* parent) {
    // normalize path
    auto filepath = QDir(filename);
    if (!QFileInfo::exists(filepath.path())) {
        QMessageBox::warning(parent,
                             QApplication::applicationDisplayName(),
                             tr("File %1 not found!").arg(filepath.path()));
        return false;
    }

    // System representation
    auto path = filepath.path();
    auto it = mOpenedProjects.find(path);
    if (it != mOpenedProjects.end()) {
        // Open existing opened project
        it->second->setFocus();
        it->second->activateWindow();
        return true;
    }

    // Get free window or create new
    if (mAvailableWindows.empty()) {
        createMainWindow();
    }

    MainWindow* newWindow = mAvailableWindows.back();
    mAvailableWindows.pop_back();

    // Init project in window
    if (!newWindow->initProject(path)) {
        delete newWindow;
        return false;
    }

    mOpenedProjects.insert({path, newWindow});
    // todo https://forum.qt.io/topic/126868/i-need-to-include-qtplatformheaders-qwindowswindowfunctions/3
    newWindow->show();
    return true;
}

void ApplicationProjectController::closeProject(const QString &filename) {
    mOpenedProjects.erase(filename);
}
