//
// Created by darkp on 06.10.2022.
//

#ifndef FFI_RUST_REACHABILITY_WINDOW_H
#define FFI_RUST_REACHABILITY_WINDOW_H

#include <QWidget>
#include <DockManager.h>

namespace ffi {
    struct PetriNet;
}

namespace rust {
    struct Reachability;
}

class ReachabilityWindow : public QWidget {

public:

    explicit ReachabilityWindow(ffi::PetriNet*, rust::Reachability*, QWidget* parent = nullptr);

private:

    ads::CDockManager* m_manager;
    ffi::PetriNet* m_net;
    rust::Reachability* m_reachability;

    ads::CDockWidget* m_view;
};


#endif //FFI_RUST_REACHABILITY_WINDOW_H
