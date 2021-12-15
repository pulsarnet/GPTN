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

    explicit PetriObject(QGraphicsItem* parent = nullptr) : QGraphicsItem(parent) {
        setFlags(ItemIsMovable | ItemSendsGeometryChanges);
    }

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            this->setCursor(QCursor(Qt::ClosedHandCursor));
        }

        Q_UNUSED(event)
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            this->setPos(mapToScene(event->pos()));
        }

        Q_UNUSED(event)
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            this->setCursor(QCursor(Qt::ArrowCursor));
        }
        Q_UNUSED(event)
    }

public:
    virtual QPointF connectionPos(qreal angle) {
        return {0, 0};
    }

    virtual QPointF center() = 0;

    virtual qreal angleBetween(const QPointF& to) {
        qreal x = center().x();
        qreal y = center().y();

        qreal deltaX = to.x() - x;
        qreal deltaY = to.y() - y;

        qreal rotation = -atan2(deltaX, deltaY);
        rotation = qDegreesToRadians(qRadiansToDegrees(rotation) + 180);

        return rotation;
    }

};

class Position : public PetriObject {

public:

    explicit Position(QGraphicsItem* parent = nullptr): PetriObject(parent) {

    }

    [[nodiscard]] QRectF boundingRect() const override {
        qreal penWidth = 1;
        qreal diameter = 2 * radius;
        return {-radius - penWidth / 2, -radius - penWidth / 2,
                      diameter + penWidth, diameter + penWidth};
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        painter->drawEllipse(boundingRect().center(), radius, radius);
    }

    QPointF center() override {
        return scenePos();
    }

    QPointF connectionPos(qreal angle) override {
        double x = scenePos().x();
        double y = scenePos().y();

        angle = angle - qDegreesToRadians(90);
        qreal xPosy = round((x + qCos(angle) * radius));
        qreal yPosy = round((y + qSin(angle) * radius));

        return {xPosy, yPosy};
    }

private:

    qreal radius = 25.0;

};

class Transition : public PetriObject {

public:

    explicit Transition(const QPointF& origin, QGraphicsItem* parent = nullptr): PetriObject(parent), m_origin(origin) {
        this->setPos(m_origin);
    }

    [[nodiscard]] QRectF boundingRect() const override {
        return {-10, -30, 20, 60};
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option)
        Q_UNUSED(widget)

        painter->drawRect(boundingRect());
    }

    QPointF center() override {
        return this->scenePos();
    }

    QPointF connectionPos(qreal angle) override {
        double x = center().x();
        double y = center().y();

        angle = angle - qDegreesToRadians(90);
        qreal xPosy = round((x + qCos(angle) * sceneBoundingRect().width() / 2.));
        qreal yPosy = round((y + qSin(angle) * sceneBoundingRect().height() / 2.));

        return {xPosy, yPosy};
    }

private:

    QPointF m_origin;

};


#endif //FFI_RUST_POSITION_H
