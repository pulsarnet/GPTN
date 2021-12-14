//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_CONNECTION_H
#define FFI_RUST_CONNECTION_H

#include <QGraphicsLineItem>

class Connection : public QGraphicsLineItem {

protected:

    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override {

    }

private:

    qreal x1, y1, x2, y2;

};


#endif //FFI_RUST_CONNECTION_H
