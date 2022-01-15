//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_PETRI_OBJECT_H
#define FFI_RUST_PETRI_OBJECT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>
#include "../ffi/methods.h"

class PetriObject : public QObject, public QGraphicsItem {

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    enum ObjectType {
        Position,
        Transition
    };

    explicit PetriObject(QGraphicsItem* parent = nullptr);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:

    virtual QPointF connectionPos(PetriObject*, bool reverse);

    virtual QPointF center() = 0;

    virtual qreal angleBetween(const QPointF& to);

    virtual uint64_t index() const = 0;

    template<typename T, typename U>
    static T* castTo(U* item) { return dynamic_cast<T*>(item); }

    virtual ObjectType objectType() = 0;

    virtual QString objectTypeStr() = 0;

    virtual bool allowConnection(PetriObject* other) = 0;

    virtual void connectTo(PetriNet* net, PetriObject* other) = 0;

    void setColored(bool colored) {
        m_colored = colored;
        emit update(boundingRect());
    }

    bool colored() { return m_colored; }

protected:

    bool m_colored = false;

};

#endif //FFI_RUST_PETRI_OBJECT_H
