//
// Created by darkp on 09.01.2023.
//

#include "InputHandler3D.h"

InputHandler3D::InputHandler3D(QObject *parent) : Q3DInputHandler(parent) {

}

void InputHandler3D::mouseDoubleClickEvent(QMouseEvent *event) {
    if (isSelectionEnabled()) {
        if (!scene()->isSlicingActive()) {
            setInputPosition(event->pos());
            scene()->setSelectionQueryPosition(event->pos());
        }
    }
    QAbstract3DInputHandler::mouseDoubleClickEvent(event);
}

void InputHandler3D::mousePressEvent(QMouseEvent *event, const QPoint &mousePos) {
    if (event->buttons() & Qt::LeftButton) {
        //qDebug() << "InputHandler3D::mousePressEvent";
    } else {
        Q3DInputHandler::mousePressEvent(event, mousePos);
    }
}
