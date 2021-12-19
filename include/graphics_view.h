//
// Created by nmuravev on 12/13/2021.
//

#ifndef FFI_RUST_GRAPHICS_VIEW_H
#define FFI_RUST_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
#include <QTransform>
#include "graphics_view_zoom.h"
#include "position.h"
#include "arrow_line.h"

class GraphicsView : public QGraphicsView {

    Q_OBJECT

public:
    enum Action {
        A_Position,
        A_Transition,
        A_Connect,
        A_Move,
        A_Rotate,
        A_Nothing
    };

public:

    GraphicsView(QWidget* parent = nullptr);

    void setAction(Action action);

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void updateConnections();

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    QGraphicsItem* itemAt(QPointF pos);

    void mouseReleaseEvent(QMouseEvent *event) override;

    QLineF connectObjects(PetriObject* from, PetriObject* to);

    Action currentAction();

    void resizeEvent(QResizeEvent *event) override;

    void saveToFile(QFile& file);

    void openFile(QFile& file);

protected slots:

    void slotSceneChanged(const QList<QRectF>& region);

signals:

    void signalSceneChanged();

private:

    typedef std::pair<QGraphicsLineItem*, std::pair<QGraphicsItem*, QGraphicsItem*>> Connection;

    QGraphicsScene* scene;
    QList<QGraphicsItem*> items;

    QList<Connection> connections;
    Connection* current_connection = nullptr;

    bool shift_pressed = false;

    Action action = Action::A_Nothing;

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;

};


#endif //FFI_RUST_GRAPHICS_VIEW_H
