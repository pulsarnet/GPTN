//
// Created by darkp on 09.01.2023.
//

#ifndef FFI_RUST_INPUTHANDLER3D_H
#define FFI_RUST_INPUTHANDLER3D_H

#include <Q3DInputHandler>

class InputHandler3D : public Q3DInputHandler {

public:

    explicit InputHandler3D(QObject* parent = nullptr);

    void mouseDoubleClickEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos) override;

};


#endif //FFI_RUST_INPUTHANDLER3D_H
