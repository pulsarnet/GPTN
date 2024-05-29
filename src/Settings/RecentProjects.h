#ifndef FFI_RUST_RECENTPROJECTS_H
#define FFI_RUST_RECENTPROJECTS_H

#include <QString>

class RecentProjects {

    struct RecentProject {
        QString path;
        QString id;
    };

public:

    static bool init();

    static std::vector<QString> getRecentProjects();
    static void addRecentProject(const QString& path);
    static void removeRecentProject(const QString& path);

private:

    static QString path();
    static std::vector<RecentProject> readRecentProjects();
    static void writeRecentProjects(std::vector<RecentProject>&& projects);

    static const QString RECENT_PROJECTS_FILE;

};

#endif //FFI_RUST_RECENTPROJECTS_H
