//
// Created by darkp on 15.09.2023.
//

#include "ProjectMetadata.h"
#include "FFI/rust.h"
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

ProjectMetadata::ProjectMetadata(const QString &fileName): m_fileName(fileName) {
    this->m_changed = false;

    // Calculate project Name
    auto path = fs::path(fileName.toStdString());
    auto shortNameFilename = path.filename().generic_string();
    size_t lastIndex = shortNameFilename.find_last_of(".");
    m_projectName = QString::fromStdString(shortNameFilename.substr(0, lastIndex));

    this->m_ctx = ffi::PetriNetContext::create();
}