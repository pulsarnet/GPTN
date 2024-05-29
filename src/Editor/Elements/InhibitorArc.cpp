#include "InhibitorArc.h"

#include "PetriObject.h"
#include "Position.h"
#include "Transition.h"
#include "../GraphicsScene.h"

InhibitorArc::InhibitorArc(Position* from, const QLineF &line, QGraphicsItem* parent)
    : Edge(from, line, parent)
{

}

InhibitorArc::InhibitorArc(Position *from, Transition *to, QGraphicsItem *parent)
    : Edge(from, to, parent)
{

}

bool InhibitorArc::allowDestination(const PetriObject *dest) const noexcept {
    return dest->vertex()->type() == vertex::Transition;
}

QVariant InhibitorArc::edgeChange(EdgeChange change, const QVariant &value) {
    if (change == EdgeSceneChanged) {
        // old scene, may be null
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        // remove from net of current scene
        auto net = s->net();
        net->remove_inhibitor(from()->vertexIndex(), to()->vertexIndex());
    } else if (change == EdgeSceneHasChanged) {
        // new scene, may be null
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        // add to net of new scene
        auto net = s->net();
        net->add_inhibitor(from()->vertexIndex(), to()->vertexIndex());
    }

    return Edge::edgeChange(change, value);
}

QPainterPath InhibitorArc::nodeShapeAt(QLineF line) const {
    static constexpr size_t radius = 5;

    double dx = -line.dx();
    double dy = -line.dy();

    double angle = sqrt(dx * dx + dy * dy);
    dx = dx / angle;
    dy = dy / angle;

    QPainterPath path;
    path.addEllipse(QPointF(line.p2().x() + dx * radius, line.p2().y() + dy * radius), radius, radius);
    return path;
}
