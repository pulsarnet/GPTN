//
// Created by Николай Муравьев on 15.01.2022.
//

#ifndef FFI_RUST_GRAPHICSITEM_H
#define FFI_RUST_GRAPHICSITEM_H

#include <QGraphicsItem>

class GraphicsItem : public QGraphicsItem {

public:

    GraphicsItem(QGraphicsItem* parent = nullptr) : QGraphicsItem(parent) {}

private:

    bool m_selected = true;

};

#endif //FFI_RUST_GRAPHICSITEM_H
