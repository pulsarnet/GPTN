#include <QEvent>
#include "reachability_tree_scene.h"
#include "reachability_node.h"
#include "reachability_line.h"

ReachabilityTreeScene::ReachabilityTreeScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_wrapper(new GraphVizWrapper)
{
    setSceneRect(-12500, -12500, 25000, 25000);
}

void ReachabilityTreeScene::addNode(QList<int32_t> data) {
    auto node = new ReachabilityNode(std::move(data));

    // setup graphviz wrapper node
    auto index = QString("s%1").arg(m_nodes.length());
    auto circle = m_wrapper->addCircle(index.toUtf8().data(), QSize(30, 30));
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
    auto edge = m_wrapper->addEdge(from_node, to_node);
    addItem(new ReachabilityLine(edge));

    m_wrapper->setEdgeLabel(edge, QString("T%1").arg(transition.id).toUtf8().data());
}

void ReachabilityTreeScene::setTree(rust::ReachabilityTree *tree) {
    m_tree = tree;
    removeAll();
    reload();
}

void ReachabilityTreeScene::removeAll() {
    for (auto item : items()) {
        removeItem(item);
    }
    m_nodes.clear();

    // recreate graphviz
    delete m_wrapper;
    m_wrapper = new GraphVizWrapper;
}

void ReachabilityTreeScene::reload() {
    auto markings = m_tree->marking();
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

            // Transition always exists
            addEdge(from, to, marking->transition());
        }
    }

    m_wrapper->setRankDir(GraphVizWrapper::BottomToTop);
    m_wrapper->save((char*)"dot");

    // Обновим все элементы
    std::for_each(m_nodes.begin(), m_nodes.end(), [](ReachabilityNode* node) {
        node->updateLayout();
    });
}