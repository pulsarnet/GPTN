//
// Created by nmuravev on 1/18/2022.
//


#include "GraphvizWrapper.h"
#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWidget>
#include <QJsonArray>

GraphVizWrapper::GraphVizWrapper() {
    m_context = gvContext();

    m_graph = agopen((char*)"g", Agdirected, 0);
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

GraphModel GraphVizWrapper::save(char* algorithm) {
    gvLayout(m_context, m_graph, algorithm);

    GraphModel net;
    for (Agnode_t* node = agfstnode(m_graph); node != NULL; node = agnxtnode(m_graph, node)) {
        auto name = QString(((Agnodeinfo_t*)AGDATA(node))->label->text);
        auto x = ((Agnodeinfo_t*)AGDATA(node))->coord.x * 1.73;
        auto y = ((Agnodeinfo_t*)AGDATA(node))->coord.y * 1.73;
        net.elements.push_back({name, QPointF(x, y)});
    }

    return net;
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
