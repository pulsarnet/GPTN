//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_TRANSITION_H
#define FFI_RUST_TRANSITION_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>

class Transition : public PetriObject {

public:

    struct TransitionState {
        int parent;
    };

    explicit Transition(const QPointF& origin,
                        ffi::PetriNet* net,
                        ffi::VertexIndex transition,
                        QGraphicsItem *parent = nullptr);

    explicit Transition(const QPointF& origin,
                        ffi::PetriNet* net,
                        ffi::VertexIndex transition,
                        TransitionState* state,
                        QGraphicsItem *parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(PetriObject*, bool reverse) override;

    ObjectType objectType() override { return PetriObject::Transition; }

    QString objectTypeStr() override { return "transition"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == Position;
    }

    virtual QString name() const override;

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

protected:

    void onAddToScene(GraphicScene*) override;
    void onRemoveFromScene() override;

private:

    void setLabelTransformation();

private:

    QPointF m_origin;
    QGraphicsTextItem* m_label;
    TransitionState* m_state = nullptr;
};

#endif //FFI_RUST_TRANSITION_H
