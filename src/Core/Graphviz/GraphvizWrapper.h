#ifndef GRAPH_GRAPHVIZ_WRAPPER_H
#define GRAPH_GRAPHVIZ_WRAPPER_H

#include <gvc/gvc.h>
#include <cgraph/cgraph.h>
#include <QPainter>
#include <qmap.h>


struct GraphModel {
    QList<std::pair<QString, QPointF>> elements;
};

class GraphVizWrapper {

public:

    enum RankDir {
        TopToBottom,
        BottomToTop,
        LeftToRight,
        RightToLeft,
    };

    explicit GraphVizWrapper();

    Agnode_s* addCircle(char* name, const QSizeF& size, const QPointF &point = QPointF(0, 0));
    Agnode_s* addRectangle(char* name, const QSizeF& size, const QPointF &point = QPointF(0, 0));

    Agedge_s* addEdge(const QString& from, const QString& to);
    Agedge_s* addEdge(Agnode_s* from, Agnode_s* to);
    void setEdgeLabel(Agedge_s*, char* label);

    void setRankDir(RankDir dir);

    GraphVizWrapper subGraph(char* name);
    GraphModel build(char* algorithm);

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
