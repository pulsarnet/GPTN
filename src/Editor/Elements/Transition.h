#ifndef FFI_RUST_TRANSITION_H
#define FFI_RUST_TRANSITION_H

#include <QGraphicsItem>
#include "PetriObject.h"

class Transition : public PetriObject {

public:

    struct TransitionState {
        int parent;
    };

    explicit Transition(const QPointF& origin,
                        net::PetriNet* net,
                        vertex::VertexIndex transition,
                        QGraphicsItem *parent = nullptr);

    explicit Transition(const QPointF& origin,
                        net::PetriNet* net,
                        vertex::VertexIndex transition,
                        TransitionState* state,
                        QGraphicsItem *parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(PetriObject*) override;

    ObjectType objectType() override { return PetriObject::Transition; }

    QString objectTypeStr() override { return "transition"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == Position;
    }

    virtual QString name() const override;

protected:

    void onAddToScene(GraphicsScene*) override;
    void onRemoveFromScene() override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

private:

    QPointF m_origin;
    TransitionState* m_state = nullptr;
    bool m_rotated = false;
};

#endif //FFI_RUST_TRANSITION_H
