//
// Created by nmuravev on 1/18/2022.
//

#ifndef GRAPH_GRAPHVIZ_WRAPPER_H
#define GRAPH_GRAPHVIZ_WRAPPER_H

#include <graphviz/gvc.h>
#include <QString>
#include <QPainter>
#include <qmap.h>


struct Net {
    QList<std::pair<QString, QPointF>> elements;
};

class GraphVizWrapper {

public:
    explicit GraphVizWrapper(int argc, char* argv[]);

    Agnode_s* addPosition(char* name);

    Agnode_s* addTransition(char* name);

    void addEdge(const QString& from, const QString& to);

    void addEdge(Agnode_s* from, Agnode_s* to);

    Net save(char* algorithm);

    ~GraphVizWrapper();

private:

    QMap<QString, Agnode_s*> m_elements;

    GVC_t* m_context;
    Agraph_t* m_graph;

};


#endif //GRAPH_GRAPHVIZ_WRAPPER_H
