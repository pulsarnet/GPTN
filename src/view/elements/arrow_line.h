//
// Created by nmuravev on 12/14/2021.
//

#ifndef FFI_RUST_ARROW_LINE_H
#define FFI_RUST_ARROW_LINE_H

#define LINE_WIDTH 2

#include <QGraphicsItem>
#include <QPainter>

namespace ffi {
    struct PetriNet;
    struct Connection;
}

class PetriObject;
class GraphicScene;

class ArrowLine : public QGraphicsLineItem {

public:

    struct ConnectionState {
        int weight;
    };

    explicit ArrowLine(PetriObject* from, const QLineF &line, QGraphicsItem* parent = nullptr);
    explicit ArrowLine(PetriObject* from, PetriObject* to, QGraphicsItem* parent = nullptr);
    explicit ArrowLine(PetriObject* from, PetriObject* to, ConnectionState* state, QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    bool setTo(PetriObject* to);

    void setBidirectional(bool b);
    bool isBidirectional() const { return m_bidirectional; }

    ffi::Connection* netItem(bool reverse = false);

    PetriObject* from() {
        return m_from;
    }

    PetriObject* to() {
        return m_to;
    }

    void updateConnection();

protected:

    QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) override;

    void onAddToScene(GraphicScene* scene);
    void onRemoveFromScene();

private:

    static QPolygonF arrow(QLineF line);

private:

    bool m_bidirectional = false;
    PetriObject* m_from = nullptr;
    PetriObject* m_to = nullptr;
    ConnectionState* m_state = nullptr;

    QPainterPath m_shape;
    QPolygonF m_arrow1;
    QPolygonF m_arrow2;
    QString m_text;
};


#endif //FFI_RUST_ARROW_LINE_H
