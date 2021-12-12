//
// Created by nmuravev on 12/12/2021.
//

#ifndef FFI_RUST_RECTANGLE_H
#define FFI_RUST_RECTANGLE_H

#include "drawable_object.h"

class Rectangle : public DrawableObject {

public:

    explicit Rectangle(QPointF origin, qreal width, qreal height) {
        this->origin = origin;
        this->width = width;
        this->height = height;
    }

    void paint(QPainter *painter) override {
        painter->drawRect(QRectF(origin.x(), origin.y(), width, height));
    }

    bool contains(const QPointF &pos) override {
        return QRectF(origin.x(), origin.y(), width, height).contains(pos);
    }

    void setPos(QPointF pos) override {
        this->origin = pos;
    }

private:

    QPointF origin;
    qreal width, height;

};


#endif //FFI_RUST_RECTANGLE_H
