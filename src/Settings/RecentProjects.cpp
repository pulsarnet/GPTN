//
// Created by darkp on 08.02.2023.
//

#include "RecentProjects.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QDir>
#include <QStandardPaths>

const QString RecentProjects::RECENT_PROJECTS_FILE = "recent_projects.json";

bool RecentProjects::init() {
    QFile file(RecentProjects::path());
    if (!file.exists()) {
        return file.open(QIODevice::WriteOnly);
    }
    return true;
}

std::vector<QString> RecentProjects::getRecentProjects() {
    auto projects = RecentProjects::readRecentProjects();
    std::vector<QString> paths;
    for (auto&& project : projects) {
        paths.push_back(project.path);
    }
    return paths;
}

void RecentProjects::addRecentProject(const QString &path) {
    Q_ASSERT(!path.isEmpty());

    auto base64 = QString::fromStdString(path.toUtf8().toBase64().toStdString());
    auto projects = RecentProjects::readRecentProjects();
    auto it = std::find_if(projects.begin(), projects.end(), [&base64](const RecentProject& project) {
        return project.id == base64;
    });

    if (it != projects.end()) {
        std::iter_swap(it, projects.begin());
    } else {
        projects.insert(projects.begin(), {path, base64});
    }

    RecentProjects::writeRecentProjects(std::move(projects));
}

QString RecentProjects::path() {
    auto defaultDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QDir(defaultDir).exists()) {
        QDir().mkpath(defaultDir);
    }

    return defaultDir + "/" + RecentProjects::RECENT_PROJECTS_FILE;
}

std::vector<RecentProjects::RecentProject> RecentProjects::readRecentProjects() {
    QFile file(RecentProjects::path());
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open recent projects file";
        return {};
    }

    auto doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isArray()) {
        return {};
    }

    auto array = doc.array();
    std::vector<RecentProjects::RecentProject> projects;
    for (auto&& project : array) {
        if (!project.isObject()) {
            qDebug() << "Invalid recent project entry";
            continue;
        }

        auto obj = project.toObject();
        if (!obj.contains("path") || !obj.contains("id")) {
            qDebug() << "Invalid recent project object";
            continue;
        }

        auto path = obj["path"].toString();
        auto id = obj["id"].toString();
        projects.push_back({path, id});
    }

    return projects;
}

void RecentProjects::writeRecentProjects(std::vector<RecentProject> &&projects) {
    QJsonArray array;
    for (auto&& project : projects) {
        QJsonObject obj;
        obj["path"] = project.path;
        obj["id"] = project.id;
        array.push_back(obj);
    }

    auto doc = QJsonDocument(array);
    QFile file(RecentProjects::path());
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open recent projects file";
        return;
    }

    file.write(doc.toJson());
    file.flush();
}