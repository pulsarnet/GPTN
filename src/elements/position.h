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
#include "petri_object.h"
#include "../ffi/rust.h"


class Position : public PetriObject {

public:

    explicit Position(const QPointF& origin, ffi::Position* position, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(PetriObject*, bool reverse) override;

    ObjectType objectType() override { return PetriObject::Position; }

    QString objectTypeStr() override { return "position"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == PetriObject::Transition;
    }

    uint64_t index() const override;

    void connectTo(ffi::PetriNet* net, PetriObject* other) override;

    void add_marker();

    void remove_marker();

    unsigned long markers();

    void setMarkers(unsigned long count);

    ffi::Position* position() {
        return m_position;
    }

    virtual QString name();

private:

    qreal radius = 25.0;
    ffi::Position* m_position = nullptr;

};


#endif //FFI_RUST_POSITION_H
