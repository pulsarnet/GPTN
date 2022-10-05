//
// Created by darkp on 05.10.2022.
//

#ifndef FFI_RUST_REACHABILITY_LINE_H
#define FFI_RUST_REACHABILITY_LINE_H

#include <QGraphicsLineItem>
#include <graphviz/gvc.h>

class ReachabilityLine: public QGraphicsLineItem {

public:
    explicit ReachabilityLine(Agedge_s* edge, QGraphicsItem* parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

private:

    Agedge_s* m_edge;
};


#endif //FFI_RUST_REACHABILITY_LINE_H
