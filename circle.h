//
// Created by nmuravev on 12/12/2021.
//

#ifndef FFI_RUST_CIRCLE_H
#define FFI_RUST_CIRCLE_H

#include "drawable_object.h"

class Circle : public DrawableObject {

public:

    explicit Circle(QPointF _center, qreal _radius) : DrawableObject(), center(_center), radius(_radius) {}

    void paint(QPainter *painter) override {

        painter->drawEllipse(this->center, radius, radius);

    }

    bool contains(const QPointF& point) override {
        qreal x = point.x() - center.x();
        qreal y = point.y() - center.y();

        return (x * x + y * y) <= radius * radius;
    }

    void setPos(QPointF pos) override {
        this->center = pos;
    }

private:

    QPointF center;
    qreal radius;

};


#endif //FFI_RUST_CIRCLE_H
