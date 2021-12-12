//
// Created by nmuravev on 12/12/2021.
//

#ifndef FFI_RUST_DRAWABLE_OBJECT_H
#define FFI_RUST_DRAWABLE_OBJECT_H

#include <QPainter>

class DrawableObject {

public:

    virtual void paint(QPainter* painter) = 0;

    virtual bool contains(const QPointF&) = 0;

    virtual void setPos(QPointF) = 0;
};


#endif //FFI_RUST_DRAWABLE_OBJECT_H
