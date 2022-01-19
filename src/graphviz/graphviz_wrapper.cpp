//
// Created by nmuravev on 1/18/2022.
//


#include "graphviz_wrapper.h"


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
    agsafeset(position, (char*)"pos", pos.toUtf8().data(), "");
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
    agsafeset(transition, (char*)"pos", pos.toUtf8().data(), "");
    m_elements.insert(name, transition);
    return transition;
}

void GraphVizWrapper::addEdge(const QString &from, const QString &to) {
    addEdge(*m_elements.find(from), *m_elements.find(to));
}

GraphModel GraphVizWrapper::save(char* algorithm) {
    gvLayout(m_context, m_graph, algorithm);

    char* result = nullptr;
    unsigned int len;
    gvRenderData(m_context, m_graph, "json", &result, &len);

    auto document = QJsonDocument::fromJson(QString::fromStdString(std::string(result, len)).toUtf8());
    auto object = document.object();
    auto map = object.toVariantMap();
    auto objects = map["objects"].toJsonArray();

    GraphModel net;

    for (auto node : objects) {

        auto nodeObject = node.toObject();
        if (!nodeObject.contains("pos")) continue;

        QString pos_str = nodeObject.value("pos").toString();
        auto split = pos_str.split(',');
        QPointF pos(split.value(0).toFloat() * 1.73, split.value(1).toFloat() * 1.73);

        QString name = nodeObject.value("name").toString();

        net.elements.push_back({name, pos});
    }

    gvFreeLayout(m_context, m_graph);

    return net;
}

void GraphVizWrapper::addEdge(Agnode_s *from, Agnode_s *to) {
    agedge(m_graph, from, to, 0, 1);
    //agsafeset(edge, (char*)"len", "1.", "");
}

GraphVizWrapper::~GraphVizWrapper() {
    if (!m_sub) {
        agclose(m_graph);
        gvFreeContext(m_context);
    }
}

GraphVizWrapper GraphVizWrapper::subGraph(char *name) {
    Agraph_t* sub = agsubg(m_graph, name, 1);
    return GraphVizWrapper(m_context, sub);
}
