//
// Created by nmuravev on 12/14/2021.
//

#ifndef FFI_RUST_ARROW_LINE_H
#define FFI_RUST_ARROW_LINE_H

#define LINE_WIDTH 2

#include <QGraphicsItem>
#include <QPainter>
#include "position.h"
#include "petri_object.h"
#include "transition.h"

class ArrowLine : public QGraphicsLineItem {

public:
    explicit ArrowLine(PetriObject* from, const QLineF &line, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool setTo(PetriObject* to);

    PetriObject* from() {
        return m_from;
    }

    PetriObject* to() {
        return m_to;
    }

    void disconnect(PetriNet* net);

    void updateConnection();

    void setBendFactor(int bf);

private:

    PetriObject* m_from = nullptr;
    PetriObject* m_to = nullptr;

    int m_bendFactor = 0;

    QPainterPath m_shape;
    QPolygonF m_arrow;
};


#endif //FFI_RUST_ARROW_LINE_H
