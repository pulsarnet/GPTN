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
        A_Remove,
        A_Nothing
    };

public:

    explicit GraphicsView(QWidget* parent = nullptr);

    void setAction(Action action);

    void updateConnections();

    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    PetriObject* itemAt(QPointF pos);

    void mouseReleaseEvent(QMouseEvent *event) override;

    QLineF connectObjects(PetriObject* from, PetriObject* to);

    Action currentAction();

    void resizeEvent(QResizeEvent *event) override;

    void saveToFile(QFile& file);

    void openFile(QFile& file);

    uint64_t getPositionIndex();

    uint64_t getTransitionIndex();

    [[nodiscard]] const QList<QGraphicsItem*>& getItems() const {
        return this->items;
    }

    [[nodiscard]] const QList<ArrowLine*>& getConnections() const {
        return this->connections;
    }

protected slots:

    void slotSceneChanged(const QList<QRectF>& region);

signals:

    void signalSceneChanged();

    void signalRemoveItem();

private:

    QGraphicsScene* scene;
    QList<QGraphicsItem *> items;

    QList<ArrowLine*> connections;
    ArrowLine* current_connection = nullptr;

    Action action = Action::A_Nothing;

    GraphicsViewZoom* zoom = nullptr;
    QPointF m_origin;

    uint64_t lastPositionIndex = 0;
    uint64_t lastTransitionIndex = 0;

};


#endif //FFI_RUST_GRAPHICS_VIEW_H
