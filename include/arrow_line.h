//
// Created by nmuravev on 12/14/2021.
//

#ifndef FFI_RUST_ARROW_LINE_H
#define FFI_RUST_ARROW_LINE_H

#include <QGraphicsItem>
#include <QPainter>
#include "elements/position.h"
#include "petri_object.h"

class ArrowLine : public QGraphicsLineItem {

public:
    explicit ArrowLine(PetriObject* from, const QLineF &line, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool setTo(PetriObject* to) {
        if (this->m_from->allowConnection(to)) {
            m_to = to;
            return true;
        }

        return false;
    }

    PetriObject* from() {
        return m_from;
    }

    PetriObject* to() {
        return m_to;
    }

private:

    PetriObject* m_from = nullptr;
    PetriObject* m_to = nullptr;

};


#endif //FFI_RUST_ARROW_LINE_H
