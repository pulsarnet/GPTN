#ifndef FFI_RUST_ARROW_LINE_H
#define FFI_RUST_ARROW_LINE_H

#define LINE_WIDTH 2

#include <QGraphicsItem>
#include <QPainter>

namespace ptn::net {
    struct PetriNet;

    namespace edge {
        struct Connection;
    }
}

class PetriObject;
class GraphicsScene;

class Edge : public QGraphicsLineItem {

public:

    enum EdgeFlag {
        EdgeHasWeight = 0x1,
        EdgeCanBidirectional = 0x2,
    };
    Q_DECLARE_FLAGS(EdgeFlags, EdgeFlag)

    enum EdgeChange {
        EdgeWeightChanged = 1,
        EdgeWeightHasChanged = 2,
        EdgeWeightReverseChanged,
        EdgeWeightReverseHasChanged,
        EdgeBidirectionalChanged,
        EdgeBidirectionalHasChanged,
        EdgeDestinationHasChanged,
        EdgeSceneChanged, // ItemSceneChange
        EdgeSceneHasChanged, // ItemSceneHasChanged
    };

    virtual const QByteArray typeId() const = 0;

    QRectF boundingRect() const override;

    void setEdgeFlags(EdgeFlags flags) { m_flags = flags; }
    EdgeFlags edgeFlags() const { return EdgeFlags(m_flags); }

    void setDestination(PetriObject* to);
    PetriObject* destination() const { return m_to; }

    void setBidirectional(bool b);
    bool isBidirectional() const { return m_bidirectional; }

    void setWeight(uint32_t weight, bool reverse = false);
    uint32_t weight(bool reverse = false) const;

    virtual bool allowDestination(const PetriObject* dest) const noexcept = 0;

    // TODO
    void blockChange(bool block = true) { m_blockChange = block; }

    PetriObject* from() const { return m_from; }
    PetriObject* to() const { return m_to; }

    void updateConnection();

protected:

    explicit Edge(PetriObject* from, const QLineF &line, QGraphicsItem* parent = nullptr);
    explicit Edge(PetriObject* from, PetriObject* to, QGraphicsItem* parent = nullptr);

    virtual QVariant edgeChange(EdgeChange change, const QVariant &value);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    virtual QPainterPath nodeShapeAt(QLineF) const = 0;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    static EdgeChange mapToChange(GraphicsItemChange change);

private:

    quint32 m_flags : 2 = 0;

    quint32 m_weight[2] = {1, 1};

    bool m_bidirectional = false;
    bool m_blockChange = false;

    PetriObject* m_from = nullptr;
    PetriObject* m_to = nullptr;

    QPainterPath m_shape;
    QPainterPath m_nodeShape[2]; // arrow form, second use for bidirectional
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Edge::EdgeFlags)

#endif //FFI_RUST_ARROW_LINE_H
