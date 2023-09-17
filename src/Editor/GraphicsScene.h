
#ifndef FFI_RUST_GRAPHICSCENE_H
#define FFI_RUST_GRAPHICSCENE_H

#include <QGraphicsScene>

class PetriObject;
class Position;
class Transition;
class ArrowLine;
class GraphicSceneActions;
class QUndoCommand;
class QUndoStack;

namespace ffi {
    struct Vertex;
    struct PetriNet;
    struct Simulation;
}

class GraphicsScene : public QGraphicsScene {

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

    explicit GraphicsScene(ffi::PetriNet* net, QObject* parent = nullptr);

    void setAllowMods(Modes mods);

    QJsonDocument json() const;
    bool fromJson(const QJsonDocument& document);

    void removeAll();

    // Operations with items
    PetriObject* netItemAt(const QPointF& pos);
    Transition* getTransition(int index);
    Position* getPosition(int index);
    void registerItem(QGraphicsItem* item);
    void unregisterItem(QGraphicsItem* item);

    // Operations with connections
    void setConnectionWeight(ArrowLine* connection, int weight, bool reverse);
    ArrowLine* getConnection(PetriObject* from, PetriObject* to);

    QUndoStack* undoStack() const { return m_undoStack; }

    [[nodiscard]] const QList<Position*>& positions() const { return m_positions; }
    [[nodiscard]] const QList<Transition*>& transitions() const { return m_transition; }
    [[nodiscard]] const QList<ArrowLine*>& connections() const { return m_connections; }

    QList<Position*>& positions() { return m_positions; }
    QList<Transition*>& transitions() { return m_transition; }
    QList<ArrowLine*>& connections() { return m_connections; }

    void dotVisualization(char* algorithm);

    ffi::PetriNet* net();

    void onSceneChanged();

    void setSimulation(ffi::Simulation*);
    ffi::Simulation* simulation() const;

    GraphicSceneActions* actions();

public slots:

    void setMode(GraphicsScene::Mode mode);
    void slotHorizontalAlignment(bool triggered);
    void slotVerticalAlignment(bool triggered);

signals:
    void sceneChanged();

protected:

    void createActions();

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void insertPosition(QGraphicsSceneMouseEvent *);
    void insertTransition(QGraphicsSceneMouseEvent *);

    void rotateObject(QGraphicsSceneMouseEvent *);
    void removeObject(QGraphicsSceneMouseEvent *event);

    void connectionStart(QGraphicsSceneMouseEvent *);
    void connectionCommit(QGraphicsSceneMouseEvent *event);
    void connectionRollback(QGraphicsSceneMouseEvent* event);

    void markPosition(QGraphicsSceneMouseEvent* event);

private:

    QUndoStack* m_undoStack;
    GraphicSceneActions* m_actions = nullptr;

    QList<Position*> m_positions;
    QList<Transition*> m_transition;
    QList<ArrowLine*> m_connections;

    ArrowLine* m_currentConnection = nullptr;

    Mode m_mode;
    Mode m_restoreMode;

    Modes m_allowMods;
    Modes m_restoreMods;

    ffi::PetriNet* m_net;

    bool m_restore = false;
    bool m_dragInProgress = false;
    QList<QPair<PetriObject*, QPointF>> m_draggedItems;

    ffi::Simulation* m_simulation = nullptr;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicsScene::Modes);

#endif //FFI_RUST_GRAPHICSCENE_H