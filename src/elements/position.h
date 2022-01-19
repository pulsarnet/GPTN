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

    void connectTo(PetriNet* net, PetriObject* other) override;

    void add_marker() {
        m_position->add_marker();
    }

    void remove_marker() {
        m_position->remove_marker();
    }

    unsigned long markers() {
        return m_position->markers();
    }

    void setMarkers(unsigned long count) {
        m_position->set_markers(count);
    }

    FFIPosition* position() {
        return m_position;
    }

    virtual QString name() {
        return QString("p%1").arg(m_position->index());
    }

private:

    qreal radius = 25.0;
    FFIPosition* m_position = nullptr;

};


#endif //FFI_RUST_POSITION_H
