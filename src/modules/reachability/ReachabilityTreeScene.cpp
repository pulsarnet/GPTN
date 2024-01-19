#include <QEvent>
#include "ReachabilityTreeScene.h"
#include "ReachabilityNode.h"
#include "ReachabilityLine.h"

ReachabilityTreeScene::ReachabilityTreeScene(QObject *parent)
    : QGraphicsScene(parent)
    , m_wrapper(new GraphVizWrapper)
{
    setSceneRect(-12500, -12500, 25000, 25000);
}

void ReachabilityTreeScene::addNode(QList<int32_t> data, ptn::modules::reachability::CovType type, std::optional<QList<ptn::net::vertex::VertexIndex>> headers) {
    const auto node = headers.has_value()
            ? new ReachabilityNode(std::move(data), std::move(headers.value()))
            : new ReachabilityNode(std::move(data));

    // setup graphviz wrapper node
    const auto index = QString("s%1").arg(m_nodes.length());
    const auto circle = m_wrapper->addCircle(index.toUtf8().data(), QSize(30, 30));
    agsafeset(circle, (char*)"shape", (char*)"record", "");
    agsafeset(circle, (char*)"label", node->text().toUtf8().data(), "");
    node->setGraphVizNode(circle);
    node->setType(type);

    m_nodes.push_back(node);
    addItem(node);
}

void ReachabilityTreeScene::addEdge(ReachabilityNode *from, ReachabilityNode *to, ptn::net::vertex::VertexIndex transition) {
    if (!from || !to) return;

    const auto from_node = from->graphVizNode();
    const auto to_node = to->graphVizNode();
    const auto edge = m_wrapper->addEdge(from_node, to_node);
    addItem(new ReachabilityLine(edge));

    m_wrapper->setEdgeLabel(edge, QString("T%1").arg(transition.id).toUtf8().data());
}

void ReachabilityTreeScene::setTree(ptn::modules::reachability::ReachabilityTree *tree) {
    m_tree->drop();
    m_tree = tree;
    removeAll();
    reload();
}

void ReachabilityTreeScene::removeAll() {
    for (const auto item : items()) {
        removeItem(item);
    }
    m_nodes.clear();

    // recreate graphviz
    delete m_wrapper;
    m_wrapper = new GraphVizWrapper;
}

void ReachabilityTreeScene::reload() {
    auto markings = m_tree->marking();
    Q_ASSERT(!markings.empty());
    bool isFirst = true;
    for (const auto marking : markings) {
        const auto prev = marking->prev();
        auto data = marking->values();

        // create node from data
        auto list = QList(data.begin(), data.end());
        if (isFirst) {
            auto indexes = m_tree->positions();
            auto headers = QList(indexes.begin(), indexes.end());
            isFirst = false;
            addNode(list, marking->type(), std::optional(headers));
        } else {
            addNode(list, marking->type());
        }
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

ReachabilityTreeScene::~ReachabilityTreeScene() {
    delete m_wrapper;
    m_tree->drop();
}