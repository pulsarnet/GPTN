#ifndef FFI_RUST_REACHABILITY_NODE_H
#define FFI_RUST_REACHABILITY_NODE_H

#include <gvc/gvc.h>
#include <QGraphicsRectItem>
#include <ptn/reachability.h>

class ReachabilityNode : public QGraphicsItem {

public:
    explicit ReachabilityNode(QList<int32_t> values, QGraphicsItem* parent = nullptr);
    explicit ReachabilityNode(QList<int32_t> values, const QList<ptn::net::vertex::VertexIndex>& headers, QGraphicsItem* parent = nullptr);

    [[nodiscard]] const QString& text() const;

    void setGraphVizNode(Agnode_s*);
    Agnode_s* graphVizNode();

    void setType(ptn::modules::reachability::CovType);
    ptn::modules::reachability::CovType getType() const;

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void updateLayout();

private:

    QList<int32_t> m_values;
    QString m_text;
    QRectF m_boundingRect;

    QPainterPath m_path;

    Agnode_s* m_node = nullptr;

    ptn::modules::reachability::CovType m_type = ptn::modules::reachability::CovType::Inner;

    void paintField(QPainter *painter, field_t *fld);
};


#endif //FFI_RUST_REACHABILITY_NODE_H
