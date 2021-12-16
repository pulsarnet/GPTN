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

    explicit PetriObject(QGraphicsItem* parent = nullptr);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

public:

    virtual QPointF connectionPos(qreal angle);

    virtual QPointF center() = 0;

    virtual qreal angleBetween(const QPointF& to);

};

class Position : public PetriObject {

public:

    explicit Position(QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(qreal angle) override;

private:

    qreal radius = 25.0;

};

class Transition : public PetriObject {

public:

    explicit Transition(const QPointF& origin, QGraphicsItem* parent = nullptr);

    [[nodiscard]] QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QPointF center() override;

    QPointF connectionPos(qreal angle) override;

private:

    QPointF m_origin;

};


#endif //FFI_RUST_POSITION_H
