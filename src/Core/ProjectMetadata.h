#ifndef GPTN_PROJECTMETADATA_H
#define GPTN_PROJECTMETADATA_H

#include <QString>

namespace ffi {
    struct PetriNetContext;
}

class ProjectMetadata {

public:

    explicit ProjectMetadata(const QString& fileName);

    const QString& fileName() const noexcept { return this->m_fileName; }
    const QString& projectName() const noexcept { return this->m_projectName; }
    ffi::PetriNetContext* context() const noexcept { return this->m_ctx; }

    void setChanged(bool flag) { this->m_changed = flag; }
    bool isChanged() const noexcept { return this->m_changed; }

private:
    QString m_fileName;
    QString m_projectName;
    bool m_changed;
    ffi::PetriNetContext* m_ctx;
};


#endif //GPTN_PROJECTMETADATA_H
