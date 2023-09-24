#ifndef GPTN_PROJECTMETADATA_H
#define GPTN_PROJECTMETADATA_H

#include <QString>

namespace ffi {
    struct PetriNetContext;
}

class ProjectMetadata {

public:

    explicit ProjectMetadata();

    [[nodiscard]] const QString& filename() const noexcept { return this->mFilename; }
    void setFilename(const QString& filename);
    [[nodiscard]] const QString& projectName() const noexcept { return this->mProjectName; }
    [[nodiscard]] ffi::PetriNetContext* context() const noexcept { return this->mCtx; }

    void setChanged(bool flag) { this->mChanged = flag; }
    [[nodiscard]] bool isChanged() const noexcept { return this->mChanged; }

private:
    QString mFilename;
    QString mProjectName;
    bool mChanged;
    ffi::PetriNetContext* mCtx;
};


#endif //GPTN_PROJECTMETADATA_H
