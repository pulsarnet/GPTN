//
// Created by Николай Муравьев on 13.01.2022.
//

#ifndef FFI_RUST_GRAPHICS_SCENE_H
#define FFI_RUST_GRAPHICS_SCENE_H

#include <QGraphicsScene>
#include "../elements/petri_object.h"
#include "../elements/position.h"
#include "../elements/transition.h"
#include "../elements/arrow_line.h"

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

        A_Default = A_Position | A_Transition | A_Connection | A_Move | A_Rotation | A_Remove | A_Nothing,
    };

    Q_DECLARE_FLAGS(Modes, Mode);

    explicit GraphicScene(QObject* parent = nullptr);

    void setAllowMods(Modes mods);

    QVariant toVariant();
    void fromVariant(const QVariant&);

    void removeAll();
    PetriObject* netItemAt(const QPointF& pos);

    Position* addPosition(const QString& name, const QPointF& point);
    Position* addPosition(int index, const QPointF& point);
    Transition* addTransition(const QString& name, const QPointF& point);
    Transition* addTransition(int index, const QPointF& point);
    void connectItems(PetriObject* from, PetriObject* to);

    [[nodiscard]] const QList<Position*>& positions() const { return m_positions; }
    [[nodiscard]] const QList<Transition*>& transitions() const { return m_transition; }
    [[nodiscard]] const QList<ArrowLine*>& connections() const { return m_connections; }

    Transition* getTransition(int index);
    Position* getPosition(int index);
    QPointF getTransitionPos(int index);
    QPointF getPositionPos(int index);

    PetriNet* net();

    void updateConnections(bool onlySelected = false);

public slots:

    void setMode(GraphicScene::Mode mode);
    void slotSelectionChanged();

    void slotHorizontalAlignment(bool triggered);
    void slotVerticalAlignment(bool triggered);

signals:
    void itemInserted(QGraphicsItem* item);
    void itemSelected(QGraphicsItem* item);


protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void insertPosition(QGraphicsSceneMouseEvent *);
    void insertTransition(QGraphicsSceneMouseEvent *);

    void rotateObject(QGraphicsSceneMouseEvent *);
    void removeObject(QGraphicsSceneMouseEvent *event);

    void removeConnectionsAssociatedWith(PetriObject*);
    void connectionStart(QGraphicsSceneMouseEvent *);
    void connectionCommit(QGraphicsSceneMouseEvent *event);
    void connectionRollback(QGraphicsSceneMouseEvent* event);

private:

    QList<Position*> m_positions;
    QList<Transition*> m_transition;
    QList<ArrowLine*> m_connections;

    ArrowLine* m_currentConnection = nullptr;

    Mode m_mod;

    Modes m_allowMods;

    PetriNet* m_net;

};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicScene::Modes)

#endif //FFI_RUST_GRAPHICS_SCENE_H