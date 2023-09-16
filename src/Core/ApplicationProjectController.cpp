//
// Created by darkp on 16.09.2023.
//

#include "ApplicationProjectController.h"
#include "../mainwindow.h"
#include <filesystem>

namespace fs = std::filesystem;

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
bool ApplicationProjectController::openProject(const QString &filename) {
    // normalize path
    auto filepath = fs::path(filename.toStdString());
    if (!fs::exists(filepath)) {
        qDebug() << "File: " << filepath << "not found!";
        return false;
    }

    // System representation
    auto systemRepresentation = QString::fromStdString(filepath.generic_string());
    auto it = mOpenedProjects.find(systemRepresentation);
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
    if (!newWindow->initProject(systemRepresentation)) {
        delete newWindow;
        return false;
    }

    mOpenedProjects.insert({systemRepresentation, newWindow});
    newWindow->show();
    return true;
}

void ApplicationProjectController::closeProject(const QString &filename) {
    mOpenedProjects.erase(filename);
}
