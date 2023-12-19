#ifndef FFI_RUST_REACHABILITY_WINDOW_H
#define FFI_RUST_REACHABILITY_WINDOW_H

#include <QWidget>

namespace ptn::net {
    struct PetriNet;
}

namespace rust {
    struct ReachabilityTree;
}

class ReachabilityView;

// todo move to ActionTabWideget directory
class ReachabilityWindow : public QWidget {

public:

    explicit ReachabilityWindow(ptn::net::PetriNet*, QWidget* parent = nullptr);

    void reload();

private:
    ptn::net::PetriNet* m_net;
    ReachabilityView* m_view;
    rust::ReachabilityTree* m_tree = nullptr;
};


#endif //FFI_RUST_REACHABILITY_WINDOW_H
