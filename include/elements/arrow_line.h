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

    void disconnect(PetriNet* net) {
        if (auto pos = dynamic_cast<Position*>(this->from()); pos) {
            net->remove_connection_p(pos->position(), dynamic_cast<Transition*>(this->to())->transition());
        }
        else if (auto transition = dynamic_cast<Transition*>(this->to()); transition) {
            net->remove_connection_t(transition->transition(), dynamic_cast<Position*>(this->to())->position());
        }
    }

private:

    PetriObject* m_from = nullptr;
    PetriObject* m_to = nullptr;

};


#endif //FFI_RUST_ARROW_LINE_H
