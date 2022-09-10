//
// Created by nmuravev on 1/18/2022.
//
#pragma once
#ifndef GRAPH_GRAPHVIZ_WRAPPER_H
#define GRAPH_GRAPHVIZ_WRAPPER_H

#include <graphviz/gvc.h>
#include <QString>
#include <QPainter>
#include <qmap.h>


struct GraphModel {
    QList<std::pair<QString, QPointF>> elements;
};

class GraphVizWrapper {

public:
    explicit GraphVizWrapper();

    Agnode_s* addCircle(char* name, const QSizeF& size, const QPointF &point = QPointF(0, 0));
    Agnode_s* addRectangle(char* name, const QSizeF& size, const QPointF &point = QPointF(0, 0));

    void addEdge(const QString& from, const QString& to);
    void addEdge(Agnode_s* from, Agnode_s* to);

    GraphVizWrapper subGraph(char* name);
    GraphModel save(char* algorithm);

    ~GraphVizWrapper();

protected:

    explicit GraphVizWrapper(GVC_t* context, Agraph_t* graph);


private:

    QMap<QString, Agnode_s*> m_elements;

    GVC_t* m_context;
    Agraph_t* m_graph;
    bool m_sub = false;

};


#endif //GRAPH_GRAPHVIZ_WRAPPER_H
