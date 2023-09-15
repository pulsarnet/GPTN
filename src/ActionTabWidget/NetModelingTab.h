//
// Created by nmuravev on 3/20/2022.
//

#ifndef FFI_RUST_NETMODELINGTAB_H
#define FFI_RUST_NETMODELINGTAB_H

#include <QWidget>
#include "../view/GraphicsView.h"

class ProjectMetadata;

class NetModelingTab : public QWidget {

public:

    explicit NetModelingTab(ProjectMetadata* metadata, QWidget *parent = nullptr);

    [[nodiscard]] ProjectMetadata* metadata() const;
    [[nodiscard]] GraphicsView* view() const;

private:

    GraphicsView *m_view;
    ProjectMetadata* m_metadata;

};


#endif //FFI_RUST_NETMODELINGTAB_H
