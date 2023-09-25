//
// Created by darkp on 28.09.2022.
//

#ifndef FFI_RUST_REACHABILITY_NODE_H
#define FFI_RUST_REACHABILITY_NODE_H

#include <graphviz/gvc.h>
#include <QGraphicsRectItem>
#include "../../Core/FFI/rust.h"
#include "../../Core/FFI/reachability.h"

class ReachabilityNode : public QGraphicsItem {

public:
    explicit ReachabilityNode(QList<int32_t> values, QGraphicsItem* parent = nullptr);
    explicit ReachabilityNode(QList<int32_t> values, QList<ffi::VertexIndex> headers, QGraphicsItem* parent = nullptr);

    [[nodiscard]] const QString& text() const;

    void setGraphVizNode(Agnode_s*);
    Agnode_s* graphVizNode();

    void setType(rust::MarkingType);
    rust::MarkingType getType() const;

    QRectF boundingRect() const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void updateLayout();

private:

    QList<int32_t> m_values;
    QString m_text;
    QRectF m_boundingRect;

    QPainterPath m_path;

    Agnode_s* m_node = nullptr;

    rust::MarkingType m_type = rust::MarkingType::Inner;

    void paintField(QPainter *painter, field_t *fld);
};


#endif //FFI_RUST_REACHABILITY_NODE_H
