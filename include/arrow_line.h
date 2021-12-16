//
// Created by nmuravev on 12/14/2021.
//

#ifndef FFI_RUST_ARROW_LINE_H
#define FFI_RUST_ARROW_LINE_H

#include <QGraphicsItem>
#include <QPainter>

class ArrowLine : public QGraphicsLineItem {

public:
    explicit ArrowLine(const QLineF& line, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

};


#endif //FFI_RUST_ARROW_LINE_H
