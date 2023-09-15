//
// Created by Николай Муравьев on 10.01.2022.
//

#ifndef FFI_RUST_PETRI_OBJECT_H
#define FFI_RUST_PETRI_OBJECT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <QGraphicsSceneMouseEvent>
#include "../../Core/FFI/rust.h"

class ArrowLine;
class GraphicScene;

class PetriObject : public QObject, public QGraphicsItem {

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    enum ObjectType {
        Position,
        Transition
    };

    explicit PetriObject(ffi::PetriNet* net, ffi::VertexIndex vertex, QGraphicsItem* parent = nullptr);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

public:

    virtual QPointF connectionPos(PetriObject*, bool reverse);

    virtual QPointF center() = 0;

    virtual qreal angleBetween(const QPointF& to);

    [[nodiscard]] uint64_t index() const;

    template<typename T, typename U>
    static T* castTo(U* item) { return dynamic_cast<T*>(item); }

    virtual ObjectType objectType() = 0;

    virtual QString objectTypeStr() = 0;

    virtual bool allowConnection(PetriObject* other) = 0;

    ffi::Vertex* vertex() const;
    ffi::VertexIndex vertexIndex() const;

    void setColored(bool colored) {
        m_colored = colored;
        emit update(boundingRect());
    }

    QString label() const;
    virtual QString name() const = 0;

    bool colored() { return m_colored; }

    void addConnectionLine(ArrowLine* line);
    void removeConnectionLine(ArrowLine* line);
    void updateLabelPosition();
    void updateConnections();
    const QList<ArrowLine*>& connections() const { return m_connections; };

    GraphicScene* graphicScene() const;

    ~PetriObject();

public slots:

    void labelChanged();

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    virtual void onAddToScene(GraphicScene*);
    virtual void onRemoveFromScene() = 0;

protected:

    bool m_colored = false;
    QList<ArrowLine*> m_connections;

    ffi::PetriNet* m_net;
    ffi::VertexIndex m_vertex;

    QGraphicsTextItem* m_labelItem;
    QGraphicsTextItem* m_name;

};

#endif //FFI_RUST_PETRI_OBJECT_H
