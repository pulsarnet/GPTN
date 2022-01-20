//
// Created by Николай Муравьев on 10.01.2022.
//

#include "../ffi/methods.h"
#include "petri_object.h"
#include "arrow_line.h"


PetriObject::PetriObject(QGraphicsItem *parent) : QGraphicsItem(parent) {
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable);
    setAcceptDrops(true);
}

void PetriObject::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }

    QGraphicsItem::mousePressEvent(event);
}

void PetriObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
    QGraphicsItem::mouseMoveEvent(event);
}

void PetriObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

QPointF PetriObject::connectionPos(PetriObject* to, bool reverse) {
    return {0, 0};
}

qreal PetriObject::angleBetween(const QPointF &to) {
    qreal x = center().x();
    qreal y = center().y();

    qreal deltaX = to.x() - x;
    qreal deltaY = to.y() - y;

    qreal rotation = -atan2(deltaX, deltaY);
    rotation = qDegreesToRadians(qRadiansToDegrees(rotation) + 180.0);

    return rotation;
}

void PetriObject::removeConnectionLine(ArrowLine* line) {
    m_connections.remove(m_connections.indexOf(line));
    updateConnections();
}

void PetriObject::addConnectionLine(ArrowLine* line) {
    if (m_connections.indexOf(line) == -1) {
        m_connections.push_back(line);
        updateConnections();
    }
}

void PetriObject::updateConnections() {

    typedef QList<ArrowLine*> LineList;
    typedef QSet<PetriObject*> Key;
    QHash<Key, LineList> lineGroups;

    for (auto conn : m_connections) {
        PetriObject* from = conn->from();
        PetriObject* to = conn->to();
        Key key = { from, to};
        lineGroups[key].append(conn);
    }

    for (const LineList& value : lineGroups) {
        if (value.count() == 1) {
            // Forward line
            value.first()->setBendFactor(0);
        }
        else {
            int bf = (value.count() & 1) ? 0 : 1;
            for (auto conn : value) {
                conn->setBendFactor(bf);
                qDebug() << "SET bf: " << bf;
                if (bf > 0) bf = 0 - bf;
                else bf = 1 - bf;
            }
        }
    }

}

PetriObject::~PetriObject() {
    for (auto connection : m_connections) {
        //connection->disconnect(net());
    }

}

QVariant PetriObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        for (auto connection : m_connections) {
            connection->updateConnection();
        }

        return value;
    }

    return QGraphicsItem::itemChange(change, value);
}
