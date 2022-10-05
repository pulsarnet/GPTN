//
// Created by darkp on 25.09.2022.
//

/// ReachabilityTreeScene не освобождает память rust::Reachability

#ifndef FFI_RUST_REACHABILITY_TREE_SCENE_H
#define FFI_RUST_REACHABILITY_TREE_SCENE_H

#include <QGraphicsScene>
#include "../../graphviz/graphviz_wrapper.h"

class ReachabilityNode;

namespace rust {
    struct Reachability;
}

class ReachabilityTreeScene : public QGraphicsScene {

public:

    explicit ReachabilityTreeScene(rust::Reachability* tree, QObject* parent = nullptr);

private:

    void addNode(QList<int32_t> data);
    void addEdge(ReachabilityNode* from, ReachabilityNode* to, int32_t transition);

private:

    QList<ReachabilityNode*> m_nodes;
    GraphVizWrapper wrapper;

};

#endif //FFI_RUST_REACHABILITY_TREE_SCENE_H
