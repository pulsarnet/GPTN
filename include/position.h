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


class PetriObject : public QObject, public QGraphicsItem {

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

    virtual QPointF connectionPos(qreal angle);

    virtual QPointF center() = 0;

    virtual qreal angleBetween(const QPointF& to);

    [[nodiscard]] uint64_t index() const { return m_index; }

    template<typename T, typename U>
    static T* castTo(U* item) { return dynamic_cast<T*>(item); }

    virtual ObjectType objectType() = 0;

    virtual QString objectTypeStr() = 0;

    virtual bool allowConnection(PetriObject* other) = 0;

    void setColored(bool colored) {
        m_colored = colored;
        emit update(boundingRect());
    }

    bool colored() { return m_colored; }

protected:

    uint64_t m_index;

    bool m_colored = false;

};

class Position : public PetriObject {

public:

    explicit Position(const QPointF& origin, uint32_t index, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(qreal angle) override;

    ObjectType objectType() override { return PetriObject::Position; }

    QString objectTypeStr() override { return "position"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == PetriObject::Transition;
    }

private:

    qreal radius = 25.0;

};

class Transition : public PetriObject {

public:

    explicit Transition(const QPointF& origin, uint32_t index, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(qreal angle) override;

    ObjectType objectType() override { return PetriObject::Transition; }

    QString objectTypeStr() override { return "transition"; }

    bool allowConnection(PetriObject *other) override {
        return other->objectType() == PetriObject::Position;
    }

private:

    QPointF m_origin;

};


#endif //FFI_RUST_POSITION_H
