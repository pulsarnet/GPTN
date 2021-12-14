//
// Created by nmuravev on 12/14/2021.
//

#ifndef FFI_RUST_ARROW_LINE_H
#define FFI_RUST_ARROW_LINE_H

#include <QGraphicsItem>
#include <QPainter>

class ArrowLine : public QGraphicsLineItem {

public:
    explicit ArrowLine(const QLineF& line, QGraphicsItem* parent = nullptr): QGraphicsLineItem(line, parent) {

    }

    QRectF boundingRect() const override {
        auto rect = QGraphicsLineItem::boundingRect();
        rect.setX(rect.x() - 6.);
        rect.setY(rect.y() - 6.);
        rect.setWidth(rect.width() + 12);
        rect.setHeight(rect.height() + 12);
        return rect;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override {
        qreal arrowSize = 10;
        painter->save();
        painter->setBrush(Qt::black);

        QLineF line(this->line().p2(), this->line().p1());

        double angle = std::atan2(-line.dy(), line.dx());
        QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,
                                              cos(angle + M_PI / 3) * arrowSize);
        QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize,
                                              cos(angle + M_PI - M_PI / 3) * arrowSize);

        QPolygonF arrowHead;
        arrowHead.clear();
        arrowHead << line.p1() << arrowP1 << arrowP2;
        painter->drawLine(line);
        painter->drawPolygon(arrowHead);
        painter->restore();
    }

};


#endif //FFI_RUST_ARROW_LINE_H
