#ifndef FFI_RUST_PETRI_OBJECT_H
#define FFI_RUST_PETRI_OBJECT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <ptn/vertex.h>

class ArrowLine;
class GraphicsScene;

class PetriObject : public QObject, public QGraphicsItem {

    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:

    enum ObjectType {
        Position,
        Transition
    };

    explicit PetriObject(ptn::net::PetriNet* net, ptn::net::vertex::VertexIndex vertex, QGraphicsItem* parent = nullptr);

protected:

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

public:

    virtual QPointF connectionPos(PetriObject*);

    virtual QPointF center() = 0;

    virtual qreal angleBetween(const QPointF& to);

    [[nodiscard]] uint64_t index() const;

    template<typename T, typename U>
    static T* castTo(U* item) { return dynamic_cast<T*>(item); }

    virtual ObjectType objectType() = 0;

    virtual QString objectTypeStr() = 0;

    virtual bool allowConnection(PetriObject* other) = 0;

    [[nodiscard]] ptn::net::vertex::Vertex* vertex() const;
    [[nodiscard]] ptn::net::vertex::VertexIndex vertexIndex() const;

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
    [[nodiscard]] const QList<ArrowLine*>& connections() const { return m_connections; };

    [[nodiscard]] GraphicsScene* graphicScene() const;

    ~PetriObject();

public slots:

    void labelChanged();

protected:

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    virtual void onAddToScene(GraphicsScene*);
    virtual void onRemoveFromScene() = 0;

protected:

    bool m_colored = false;
    QList<ArrowLine*> m_connections;

    ptn::net::PetriNet* m_net;
    ptn::net::vertex::VertexIndex m_vertex;

    QGraphicsTextItem* m_labelItem;
    QGraphicsSimpleTextItem* m_name;

};

#endif //FFI_RUST_PETRI_OBJECT_H
