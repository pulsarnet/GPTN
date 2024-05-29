/// ReachabilityTreeScene не освобождает память rust::Reachability

#ifndef FFI_RUST_REACHABILITY_TREE_SCENE_H
#define FFI_RUST_REACHABILITY_TREE_SCENE_H

#include <QGraphicsScene>
#include "../../Core/graphviz/GraphvizWrapper.h"
#include <ptn/reachability.h>

class ReachabilityNode;

namespace ptn::modules::reachability {
    struct ReachabilityTree;
}

class ReachabilityTreeScene : public QGraphicsScene {

public:

    explicit ReachabilityTreeScene(QObject* parent = nullptr);

    void setTree(ptn::modules::reachability::ReachabilityTree* tree);

    ~ReachabilityTreeScene() override;

private:

    void addNode(QList<int32_t> data, ptn::modules::reachability::CovType type, std::optional<QList<ptn::net::vertex::VertexIndex>> headers = {});
    void addEdge(ReachabilityNode* from, ReachabilityNode* to, ptn::net::vertex::VertexIndex transition);
    void removeAll();
    void reload();

private:

    QList<ReachabilityNode*> m_nodes;
    ptn::modules::reachability::ReachabilityTree* m_tree = nullptr;
    GraphVizWrapper* m_wrapper;

};

#endif //FFI_RUST_REACHABILITY_TREE_SCENE_H
