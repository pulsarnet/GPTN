#ifndef FFI_RUST_DECOMPOSEMODELTAB_H
#define FFI_RUST_DECOMPOSEMODELTAB_H

#include <DockManager.h>

#include "BaseTab.h"

class SynthesisWindow;
class QScatter3DSeries;
class ProjectMetadata;
class DockWidget;

namespace ptn::modules::decompose {
    struct DecomposeContext;
}

class DecomposeModelTab : public BaseTab {

public:

    explicit DecomposeModelTab(ProjectMetadata* metadata, QWidget *parent = nullptr);

public slots:

    void selectedPoint(int idx);

protected:

    ptn::modules::decompose::DecomposeContext* decomposeContext() const noexcept;

private:

    ProjectMetadata* m_metadata;

    ads::CDockManager* m_dockManager;
    DockWidget* m_linearBaseFragmentsView;
    DockWidget* m_primitiveNetView;
    DockWidget* m_plotWidget;

    QScatter3DSeries* m_series;

    QHash<size_t, SynthesisWindow*> m_synthesisWindows;
    QHash<QVector3D, QVector<std::size_t>> m_graphPoints;
};


#endif //FFI_RUST_DECOMPOSEMODELTAB_H
