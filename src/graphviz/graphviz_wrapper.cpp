//
// Created by nmuravev on 1/18/2022.
//


#include "graphviz_wrapper.h"


#include <QObject>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWidget>
#include <QJsonArray>
GraphVizWrapper::GraphVizWrapper(int argc, char **argv) {
    m_context = gvContext();

    m_graph = agopen((char*)"g", Agdirected, 0);
    agsafeset(m_graph, (char*)"splines", "line", "");
    //agsafeset(m_graph, (char*)"overlap", "scalexy", "");

    //gvParseArgs(m_context, argc, argv);
}

Agnode_s *GraphVizWrapper::addPosition(char *name) {
    auto position = agnode(m_graph, name, 1);
    agsafeset(position, (char*)"width", (char*)"0.5", "");
    agsafeset(position, (char*)"height", (char*)"0.5", "");
    m_elements.insert(name, position);
    return position;
}

Agnode_s *GraphVizWrapper::addTransition(char *name) {
    auto transition = agnode(m_graph, name, 1);
    agsafeset(transition, (char*)"shape", (char*)"rectangle", "");
    agsafeset(transition, (char*)"width", (char*)"0.5", "");
    agsafeset(transition, (char*)"height", (char*)"0.25", "");
    m_elements.insert(name, transition);
    return transition;
}

void GraphVizWrapper::addEdge(const QString &from, const QString &to) {
    addEdge(*m_elements.find(from), *m_elements.find(to));
}

Net GraphVizWrapper::save(char* algorithm) {
    gvLayout(m_context, m_graph, algorithm);

    char* result = nullptr;
    unsigned int len;
    gvRenderData(m_context, m_graph, "json", &result, &len);

    auto document = QJsonDocument::fromJson(QString::fromStdString(std::string(result, len)).toUtf8());
    auto object = document.object();
    auto map = object.toVariantMap();
    auto objects = map["objects"].toJsonArray();

    Net net;

    for (auto node : objects) {
        QString name = node.toObject().value("name").toString();
        QString pos_str = node.toObject().value("pos").toString();

        auto split = pos_str.split(',');
        QPointF pos(split.value(0).toFloat() * 1.73, split.value(1).toFloat() * 1.73);

        net.elements.push_back({name, pos});
    }

    gvFreeLayout(m_context, m_graph);

    return net;
}

void GraphVizWrapper::addEdge(Agnode_s *from, Agnode_s *to) {
    auto edge = agedge(m_graph, from, to, 0, 1);
    //agsafeset(edge, (char*)"len", "1.", "");
}

GraphVizWrapper::~GraphVizWrapper() {
    agclose(m_graph);
    gvFreeContext(m_context);
}
