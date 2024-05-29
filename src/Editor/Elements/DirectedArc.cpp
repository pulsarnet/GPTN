#include "DirectedArc.h"

#include <ptn/net.h>

#include "PetriObject.h"
#include "../GraphicsScene.h"

DirectedArc::DirectedArc(PetriObject *from, const QLineF &line, QGraphicsItem *parent)
    : Edge(from, line, parent)
{
    setEdgeFlags(EdgeCanBidirectional | EdgeHasWeight);
}

DirectedArc::DirectedArc(PetriObject *from, PetriObject *to, QGraphicsItem *parent)
    : Edge(from, to, parent)
{
    setEdgeFlags(EdgeCanBidirectional | EdgeHasWeight);
}

bool DirectedArc::allowDestination(const PetriObject *dest) const noexcept {
    if (!dest) return false;
    return from()->vertex()->type() != dest->vertex()->type();
}

QVariant DirectedArc::edgeChange(EdgeChange change, const QVariant &value) {
    if (change == EdgeSceneChanged) {
        // old scene, may be null
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        // remove from net of current scene
        auto net = s->net();
        net->remove_directed(from()->vertexIndex(), to()->vertexIndex());
        if (isBidirectional())
            net->remove_directed(to()->vertexIndex(), from()->vertexIndex());
    } else if (change == EdgeSceneHasChanged) {
        // new scene, may be null
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        // add to net of new scene
        auto net = s->net();
        net->add_directed(from()->vertexIndex(), to()->vertexIndex(), weight());
        if (isBidirectional())
            net->add_directed(to()->vertexIndex(), from()->vertexIndex(), weight(true));
    } else if (change == EdgeWeightHasChanged || change == EdgeWeightReverseHasChanged) {
        // current scene, may be null
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        const auto net = s->net();
        const auto arc = change == EdgeWeightHasChanged ?
            net->directed_arc(from()->vertexIndex(), to()->vertexIndex())
                : net->directed_arc(to()->vertexIndex(), from()->vertexIndex());

        // set weight
        arc->set_weight(value.toUInt());
    } else if (change == EdgeBidirectionalHasChanged) {
        // current scene, may be null
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        auto net = s->net();
        if (value.toBool()) {
            net->add_directed(to()->vertexIndex(), from()->vertexIndex(), weight(true));
        } else {
            net->remove_directed(to()->vertexIndex(), from()->vertexIndex());
        }
    }

    return Edge::edgeChange(change, value);
}

QPainterPath DirectedArc::nodeShapeAt(QLineF line) const {
    static const size_t arrowSize = 10;
    double angle = std::atan2(line.dy(), -line.dx());
    QPointF arrowP1 = line.p2() + QPointF(sin(angle + M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI / 3.) * arrowSize);
    QPointF arrowP2 = line.p2() + QPointF(sin(angle + M_PI - M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI - M_PI / 3.) * arrowSize);
    QPolygonF arrow;
    arrow << line.p2() << arrowP1 << arrowP2;

    QPainterPath path;
    path.addPolygon(arrow);
    return path;
}

