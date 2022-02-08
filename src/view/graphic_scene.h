
#ifndef FFI_RUST_GRAPHIC_SCENE_H
#define FFI_RUST_GRAPHIC_SCENE_H

#include <QGraphicsScene>

class PetriObject;
class Position;
class Transition;
class ArrowLine;

namespace ffi {
    struct Vertex;
}

class GraphicScene : public QGraphicsScene {

    Q_OBJECT

public:
    enum Mode {
        A_Position = 1 << 0,
        A_Transition = 1 << 1,
        A_Connection = 1 << 2,
        A_Move = 1 << 3,
        A_Remove = 1 << 4,
        A_Rotation = 1 << 5,
        A_Nothing = 1 << 6,
        A_Marker = 1 << 7,

        A_Default = A_Position | A_Transition | A_Connection | A_Move | A_Rotation | A_Remove | A_Nothing | A_Marker
    };

    Q_DECLARE_FLAGS(Modes, Mode);

    explicit GraphicScene(ffi::PetriNet* net, QObject* parent = nullptr);
    explicit GraphicScene(const QVariant&, ffi::PetriNet*, QObject* parent = nullptr);

    void setAllowMods(Modes mods);

    QVariant toVariant();

    void removeAll();
    PetriObject* netItemAt(const QPointF& pos);

    Position* addPosition(int index, const QPointF& point);
    Position* addPosition(ffi::Vertex* position, const QPointF& point);
    Transition* addTransition(int index, const QPointF& point);
    Transition* addTransition(ffi::Vertex* transition, const QPointF& point);
    ArrowLine* connectItems(PetriObject* from, PetriObject* to, bool no_add = false);

    [[nodiscard]] const QList<Position*>& positions() const { return m_positions; }
    [[nodiscard]] const QList<Transition*>& transitions() const { return m_transition; }
    [[nodiscard]] const QList<ArrowLine*>& connections() const { return m_connections; }

    PetriObject* getVertex(const QString& name);
    Transition* getTransition(int index);
    Position* getPosition(int index);
    QPointF getTransitionPos(int index);
    QPointF getPositionPos(int index);

    void dotVisualization(char* algorithm);

    ffi::PetriNet* net();

public slots:

    void setMode(GraphicScene::Mode mode);
    void slotHorizontalAlignment(bool triggered);
    void slotVerticalAlignment(bool triggered);

signals:
    void itemInserted(QGraphicsItem* item);
    void itemRemoved();

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void insertPosition(QGraphicsSceneMouseEvent *);
    void insertTransition(QGraphicsSceneMouseEvent *);

    void rotateObject(QGraphicsSceneMouseEvent *);
    void removeObject(QGraphicsSceneMouseEvent *event);

    void removeConnectionsAssociatedWith(PetriObject*);
    void connectionStart(QGraphicsSceneMouseEvent *);
    void connectionCommit(QGraphicsSceneMouseEvent *event);
    void connectionRollback(QGraphicsSceneMouseEvent* event);

    void markPosition(QGraphicsSceneMouseEvent* event);

private:

    QList<Position*> m_positions;
    QList<Transition*> m_transition;
    QList<ArrowLine*> m_connections;

    ArrowLine* m_currentConnection = nullptr;

    Mode m_mod;

    Modes m_allowMods;

    ffi::PetriNet* m_net;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicScene::Modes);

#endif //FFI_RUST_GRAPHIC_SCENE_H
