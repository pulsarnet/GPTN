#ifndef GPTN_PROJECTMETADATA_H
#define GPTN_PROJECTMETADATA_H

#include <QString>

namespace ptn {
    namespace net {
        struct PetriNet;

        namespace context {
            struct PetriNetContext;
        }
    }

    namespace modules::decompose {
        struct DecomposeContext;
    }
}


class ProjectMetadata {

public:

    explicit ProjectMetadata();

    [[nodiscard]] const QString& filename() const noexcept { return this->mFilename; }
    void setFilename(const QString& filename);
    [[nodiscard]] const QString& projectName() const noexcept { return this->mProjectName; }

    [[nodiscard]] ptn::net::PetriNet* net() const noexcept;

    void decompose();
    [[nodiscard]] ptn::modules::decompose::DecomposeContext* decompose_ctx() const noexcept;

    void setChanged(bool flag) { this->mChanged = flag; }
    [[nodiscard]] bool isChanged() const noexcept { return this->mChanged; }

private:
    QString mFilename;
    QString mProjectName;
    bool mChanged;
    ptn::net::context::PetriNetContext* mCtx;
};


#endif //GPTN_PROJECTMETADATA_H
