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
#include "../ffi/rust.h"

class Transition : public PetriObject {

public:

    explicit Transition(const QPointF& origin, ffi::Transition* transition, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(PetriObject*, bool reverse) override;

    ObjectType objectType() override { return PetriObject::Transition; }

    QString objectTypeStr() override { return "transition"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == Position;
    }

    virtual uint64_t index() const override;

    void connectTo(ffi::PetriNet* net, PetriObject* other) override;

    ffi::Transition* transition() {
        return m_transition;
    }

    virtual QString name();

private:

    QPointF m_origin;
    ffi::Transition* m_transition = nullptr;

};

#endif //FFI_RUST_TRANSITION_H
