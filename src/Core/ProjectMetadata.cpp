//
// Created by darkp on 15.09.2023.
//

#include "ProjectMetadata.h"
#include "FFI/rust.h"
#include <filesystem>
#include <string>
#include <QDir>

namespace fs = std::filesystem;

ProjectMetadata::ProjectMetadata(const QString &filename): mFilename(filename) {
    this->mChanged = false;

    // Calculate project Name
    auto path = fs::path(filename.toStdString());
    auto shortNameFilename = path.filename().generic_string();
    size_t lastIndex = shortNameFilename.find_last_of('.');
    mProjectName = QString::fromStdString(shortNameFilename.substr(0, lastIndex));

    this->mCtx = ffi::PetriNetContext::create();
}