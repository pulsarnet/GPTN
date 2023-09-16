//
// Created by darkp on 15.09.2023.
//

#include "ProjectMetadata.h"
#include "FFI/rust.h"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

ProjectMetadata::ProjectMetadata(const QString &fileName): mFilename(fileName) {
    this->mChanged = false;

    // Calculate project Name
    auto path = fs::path(fileName.toStdString());
    auto shortNameFilename = path.filename().generic_string();
    size_t lastIndex = shortNameFilename.find_last_of('.');
    mProjectName = QString::fromStdString(shortNameFilename.substr(0, lastIndex));

    this->mCtx = ffi::PetriNetContext::create();
}