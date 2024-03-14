#include "GraphvizWrapper.h"
#include <QJsonDocument>
#include <QWidget>

GraphVizWrapper::GraphVizWrapper() {
    m_context = gvContext();

    m_graph = agopen((char*)"g", Agdirected, NULL);
    agsafeset(m_graph, (char*)"splines", "line", "");
    agsafeset(m_graph, (char*)"overlap", "scalexy", "");
    setRankDir(LeftToRight);
}

GraphVizWrapper::GraphVizWrapper(GVC_t *context, Agraph_t *graph) {
    m_context = context;
    m_graph = graph;
    m_sub = true;

    agsafeset(m_graph, (char*)"overlap", "scalexy", "");
}


Agnode_s *GraphVizWrapper::addCircle(char *name, const QSizeF &size, const QPointF &point) {
    auto position = agnode(m_graph, name, 1);

    auto width = QString("%1").arg(size.width() / 96.);
    auto height = QString("%1").arg(size.height() / 96.);
    auto pos = QString("%1,%2").arg(point.x()).arg(point.y());

    agsafeset(position, (char*)"width", width.toUtf8().data(), "");
    agsafeset(position, (char*)"height", height.toUtf8().data(), "");
    //agsafeset(position, (char*)"pos", pos.toUtf8().data(), "");
    m_elements.insert(name, position);
    return position;
}

Agnode_s *GraphVizWrapper::addRectangle(char *name, const QSizeF &size, const QPointF &point) {
    auto transition = agnode(m_graph, name, 1);

    auto width = QString("%1").arg(size.width() / 96.);
    auto height = QString("%1").arg(size.height() / 96.);
    auto pos = QString("%1,%2").arg(point.x()).arg(point.y());

    agsafeset(transition, (char*)"shape", (char*)"rectangle", "");
    agsafeset(transition, (char*)"width", width.toUtf8().data(), "");
    agsafeset(transition, (char*)"height", height.toUtf8().data(), "");
    //agsafeset(transition, (char*)"pos", pos.toUtf8().data(), "");
    m_elements.insert(name, transition);
    return transition;
}

Agedge_s* GraphVizWrapper::addEdge(const QString &from, const QString &to) {
    return addEdge(*m_elements.find(from), *m_elements.find(to));
}

Agedge_s* GraphVizWrapper::addEdge(Agnode_s *from, Agnode_s *to) {
    return agedge(m_graph, from, to, 0, 1);
}

void GraphVizWrapper::setEdgeLabel(Agedge_s *node, char *label) {
    agsafeset(node, (char*)"label", label, "");
}

void GraphVizWrapper::setRankDir(GraphVizWrapper::RankDir dir) {
    switch (dir) {
        case TopToBottom:
            agsafeset(m_graph, (char*)"rankdir", "TB", "");
            break;
        case BottomToTop:
            agsafeset(m_graph, (char*)"rankdir", "BT", "");
            break;
        case LeftToRight:
            agsafeset(m_graph, (char*)"rankdir", "LR", "");
            break;
        case RightToLeft:
            agsafeset(m_graph, (char*)"rankdir", "RL", "");
            break;
    }
}

GraphModel GraphVizWrapper::build(char* algorithm) {
    gvLayout(m_context, m_graph, algorithm);

    GraphModel model;
    Agnode_t* node;;
    for (node = agfstnode(m_graph); node; node = agnxtnode(m_graph, node)) {
        QString name;
        if (ND_label(node) && ND_label(node)->text) {
            name = QString(ND_label(node)->text);
        }
        auto x = ND_coord(node).x * 1.73;
        auto y = ND_coord(node).y * 1.73;
        model.elements.push_back({name, QPointF(x, y)});
    }

    return model;
}

GraphVizWrapper::~GraphVizWrapper() {
    if (!m_sub) {
        gvFreeLayout(m_context, m_graph);
        agclose(m_graph);
        gvFreeContext(m_context);
    }
}

GraphVizWrapper GraphVizWrapper::subGraph(char *name) {
    Agraph_t* sub = agsubg(m_graph, name, 1);
    return GraphVizWrapper(m_context, sub);
}
