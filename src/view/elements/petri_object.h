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
#include "../../ffi/rust.h"

class ArrowLine;


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

    void connectTo(ffi::PetriNet* net, PetriObject* other);

    ffi::Vertex* vertex() const;
    ffi::VertexIndex vertexIndex() const;

    void setColored(bool colored) {
        m_colored = colored;
        emit update(boundingRect());
    }

    void setLabel(const QString& label);
    QString label() const;
    virtual QString name() const = 0;

    bool colored() { return m_colored; }

    void addConnectionLine(ArrowLine* line);
    void removeConnectionLine(ArrowLine* line);
    void updateLabelPosition();
    void updateConnections();
    const QList<ArrowLine*>& connections() const { return m_connections; };

    ~PetriObject();

public slots:

    void labelChanged();

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    bool m_colored = false;
    QList<ArrowLine*> m_connections;

    ffi::PetriNet* m_net;
    ffi::VertexIndex m_vertex;

    QGraphicsTextItem* m_labelItem;

};

#endif //FFI_RUST_PETRI_OBJECT_H
