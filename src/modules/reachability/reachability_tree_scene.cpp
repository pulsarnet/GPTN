//
// Created by darkp on 25.09.2022.
//

#include <QEvent>
#include "reachability_tree_scene.h"
#include "reachability_node.h"
#include "reachability_line.h"

ReachabilityTreeScene::ReachabilityTreeScene(rust::ReachabilityTree *tree, QObject *parent)
    : QGraphicsScene(parent)
{
    setSceneRect(-12500, -12500, 25000, 25000);

    auto markings = tree->marking();
    for (auto marking : markings) {
        auto prev = marking->prev();
        auto data = marking->values();

        // create node from data
        auto list = QList(data.begin(), data.end());
        addNode(list);
        // if prev != -1 then set line
        if (prev >= 0) {
            // create connection
            auto from = m_nodes[prev];
            auto to = m_nodes.last();

            // Переход есть всегда
            addEdge(from, to, marking->transition());
        }
    }

    wrapper.setRankDir(GraphVizWrapper::BottomToTop);
    wrapper.save((char*)"dot");

    // Обновим все элементы
    std::for_each(m_nodes.begin(), m_nodes.end(), [](ReachabilityNode* node) {
        node->updateLayout();
    });
}

void ReachabilityTreeScene::addNode(QList<int32_t> data) {
    auto node = new ReachabilityNode(std::move(data));

    // setup graphviz wrapper node
    auto index = QString("s%1").arg(m_nodes.length());
    auto circle = wrapper.addCircle(index.toUtf8().data(), QSize(30, 30));
    agsafeset(circle, (char*)"shape", (char*)"record", "");
    agsafeset(circle, (char*)"label", node->text().toUtf8().data(), "");
    node->setGraphVizNode(circle);

    m_nodes.push_back(node);
    addItem(node);
}

void ReachabilityTreeScene::addEdge(ReachabilityNode *from, ReachabilityNode *to, ffi::VertexIndex transition) {
    if (!from || !to) return;

    auto from_node = from->graphVizNode();
    auto to_node = to->graphVizNode();
    auto edge = wrapper.addEdge(from_node, to_node);
    addItem(new ReachabilityLine(edge));

    wrapper.setEdgeLabel(edge, QString("T%1").arg(transition.id).toUtf8().data());
}
