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
    return window;
}

void ApplicationProjectController::emptyProject() {
    auto window = createMainWindow();
    mOpenedProjects.insert({QString(), window});
    window->show();
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

    // Use current window or create new
    bool isCurrentWindow = true;
    MainWindow* window = parent;
    if (parent->isWindowModified() || !parent->metadata()->filename().isEmpty()) {
        window = createMainWindow();
        isCurrentWindow = false;
    } else {
        mOpenedProjects.erase(window->metadata()->filename());
    }

    // Init project in window
    if (!window->initProject(path)) {
        if (!isCurrentWindow) {
            delete window;
        }
        return false;
    }

    mOpenedProjects.insert({path, window});
    // todo https://forum.qt.io/topic/126868/i-need-to-include-qtplatformheaders-qwindowswindowfunctions/3
    window->show();
    return true;
}

void ApplicationProjectController::closeProject(const QString &filename) {
    mOpenedProjects.erase(filename);
}

const std::unordered_map<QString, MainWindow*>& ApplicationProjectController::openedProjects() const {
    return mOpenedProjects;
}
