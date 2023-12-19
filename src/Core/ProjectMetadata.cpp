#include "ProjectMetadata.h"
#include <filesystem>
#include <string>
#include <QDir>

namespace fs = std::filesystem;

ProjectMetadata::ProjectMetadata() {
    this->mChanged = false;
    this->mCtx = ffi::PetriNetContext::create();
    this->mProjectName = "Untitled";
}

void ProjectMetadata::setFilename(const QString &filename) {
    // calculate project name
    auto path = fs::path(filename.toStdString());
    auto shortNameFilename = path.filename().generic_string();
    size_t lastIndex = shortNameFilename.find_last_of('.');
    mProjectName = QString::fromStdString(shortNameFilename.substr(0, lastIndex));

    // set variable
    mFilename = filename;
}