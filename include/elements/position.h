//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_POSITION_H
#define FFI_RUST_POSITION_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>
#include "../ffi/net.h"
#include "petri_object.h"


class Position : public PetriObject {

public:

    explicit Position(const QPointF& origin, FFIPosition* position, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(PetriObject*, bool reverse) override;

    ObjectType objectType() override { return PetriObject::Position; }

    QString objectTypeStr() override { return "position"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == PetriObject::Transition;
    }

    virtual uint64_t index() const override {
        return m_position->index();
    }

    FFIPosition* position() {
        return m_position;
    }

private:

    qreal radius = 25.0;
    FFIPosition* m_position = nullptr;

};


#endif //FFI_RUST_POSITION_H
