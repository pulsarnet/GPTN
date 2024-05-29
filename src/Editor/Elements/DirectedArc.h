#ifndef DIRECTEDARC_H
#define DIRECTEDARC_H

#include "Edge.h"

class PetriObject;

class DirectedArc : public Edge {

public:

    const QByteArray typeId() const override { return "directed_arc"; }

    explicit DirectedArc(PetriObject* from, const QLineF &line, QGraphicsItem* parent = nullptr);
    explicit DirectedArc(PetriObject *from, PetriObject *to, QGraphicsItem *parent = nullptr);

    bool allowDestination(const PetriObject *dest) const noexcept override;

protected:

    QVariant edgeChange(EdgeChange change, const QVariant &value) override;

    QPainterPath nodeShapeAt(QLineF) const override;

private:

    QPainterPath m_shape;
    QPolygonF m_arrow1;
    QPolygonF m_arrow2;
    QString m_text;
};



#endif //DIRECTEDARC_H
