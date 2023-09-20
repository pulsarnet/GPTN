//
// Created by darkp on 25.09.2022.
//

/// ReachabilityTreeScene не освобождает память rust::Reachability

#ifndef FFI_RUST_REACHABILITY_TREE_SCENE_H
#define FFI_RUST_REACHABILITY_TREE_SCENE_H

#include <QGraphicsScene>
#include "../../Core/graphviz/GraphvizWrapper.h"
#include "../../Core/FFI/reachability.h"

class ReachabilityNode;

namespace rust {
    struct ReachabilityTree;
}

class ReachabilityTreeScene : public QGraphicsScene {

public:

    explicit ReachabilityTreeScene(QObject* parent = nullptr);

    void setTree(rust::ReachabilityTree* tree);

private:

    void addNode(QList<int32_t> data, std::optional<QList<ffi::VertexIndex>> headers = {});
    void addEdge(ReachabilityNode* from, ReachabilityNode* to, ffi::VertexIndex transition);
    void removeAll();
    void reload();

private:

    QList<ReachabilityNode*> m_nodes;
    rust::ReachabilityTree* m_tree = nullptr;
    GraphVizWrapper* m_wrapper;

};

#endif //FFI_RUST_REACHABILITY_TREE_SCENE_H
