#ifndef INHIBITORARC_H
#define INHIBITORARC_H

#include "Edge.h"

class Position;
class Transition;

class InhibitorArc : public Edge {

public:
    const QByteArray typeId() const override { return "inhibitor_arc"; }

    explicit InhibitorArc(Position* from, const QLineF &line, QGraphicsItem* parent = nullptr);
    explicit InhibitorArc(Position* from, Transition *to, QGraphicsItem *parent = nullptr);

    bool allowDestination(const PetriObject *dest) const noexcept override;

protected:

    QVariant edgeChange(EdgeChange change, const QVariant &value) override;

    QPainterPath nodeShapeAt(QLineF) const override;
};



#endif //INHIBITORARC_H
