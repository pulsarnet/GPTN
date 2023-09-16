#ifndef GPTN_PROJECTMETADATA_H
#define GPTN_PROJECTMETADATA_H

#include <QString>

namespace ffi {
    struct PetriNetContext;
}

class ProjectMetadata {

public:

    explicit ProjectMetadata(const QString& fileName);

    const QString& fileName() const noexcept { return this->mFilename; }
    const QString& projectName() const noexcept { return this->mProjectName; }
    ffi::PetriNetContext* context() const noexcept { return this->mCtx; }

    void setChanged(bool flag) { this->mChanged = flag; }
    bool isChanged() const noexcept { return this->mChanged; }

private:
    QString mFilename;
    QString mProjectName;
    bool mChanged;
    ffi::PetriNetContext* mCtx;
};


#endif //GPTN_PROJECTMETADATA_H
