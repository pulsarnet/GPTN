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
    struct ReachabilityTree;
}

class ReachabilityView;

// todo move to ActionTabWideget directory
class ReachabilityWindow : public QWidget {

public:

    explicit ReachabilityWindow(ffi::PetriNet*, QWidget* parent = nullptr);

    void reload();

private:
    ffi::PetriNet* m_net;
    ReachabilityView* m_view;
    rust::ReachabilityTree* m_tree = nullptr;
};


#endif //FFI_RUST_REACHABILITY_WINDOW_H
