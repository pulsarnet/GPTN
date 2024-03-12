#ifndef FFI_RUST_POSITION_H
#define FFI_RUST_POSITION_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>
#include "PetriObject.h"
#include <ptn/net.h>

namespace net = ptn::net;
namespace vertex = net::vertex;

class Position : public PetriObject {

public:

    struct PositionState {
        int markers;
        int parent;
    };

    explicit Position(const QPointF& origin, net::PetriNet* net, vertex::VertexIndex position, QGraphicsItem* parent = nullptr);

    explicit Position(const QPointF& origin, net::PetriNet* net, vertex::VertexIndex position, PositionState* state, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    QPainterPath shape() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(PetriObject*) override;

    ObjectType objectType() override { return PetriObject::Position; }

    QString objectTypeStr() override { return "position"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == PetriObject::Transition;
    }

    size_t markers() const;
    void setMarkers(int markers);

    QString name() const override;

protected:

    void onAddToScene(GraphicsScene* scene) override;
    void onRemoveFromScene() override;

private:

    qreal radius = 25.0;
    PositionState* m_state = nullptr;
};


#endif //FFI_RUST_POSITION_H
