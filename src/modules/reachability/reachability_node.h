//
// Created by darkp on 28.09.2022.
//

#ifndef FFI_RUST_REACHABILITY_NODE_H
#define FFI_RUST_REACHABILITY_NODE_H

#include <graphviz/gvc.h>
#include <QGraphicsRectItem>

class ReachabilityNode : public QGraphicsItem {

public:
    explicit ReachabilityNode(QList<int32_t> values, QGraphicsItem* parent = nullptr);

    [[nodiscard]] const QString& text() const;

    void setGraphVizNode(Agnode_s*);
    Agnode_s* graphVizNode();

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void updateLayout();

private:

    QList<int32_t> m_values;
    QString m_text;
    QRectF m_boundingRect;

    Agnode_s* m_node;
};


#endif //FFI_RUST_REACHABILITY_NODE_H
