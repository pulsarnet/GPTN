//
// Created by Николай Муравьев on 10.01.2022.
//

#include <QTextDocument>
#include <QApplication>
#include "../GraphicScene.h"
#include "petri_object.h"
#include "arrow_line.h"
#include "../../ffi/rust.h"


PetriObject::PetriObject(ffi::PetriNet* net, ffi::VertexIndex _vertex, QGraphicsItem* parent) : QGraphicsItem(parent), m_net(net), m_vertex(_vertex) {
    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable | ItemUsesExtendedStyleOption);

    m_labelItem = new QGraphicsTextItem(vertex()->get_name(false), this);
    m_labelItem->setFlags(m_labelItem->flags() & 0);
    m_labelItem->setCacheMode(DeviceCoordinateCache);
    m_labelItem->setTextInteractionFlags(Qt::TextInteractionFlag::TextEditable);
    m_labelItem->setAcceptHoverEvents(false);

    connect(m_labelItem->document(), &QTextDocument::contentsChanged, this, &PetriObject::labelChanged);

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
}

void PetriObject::addConnectionLine(ArrowLine* line) {
    if (m_connections.indexOf(line) == -1) {
        m_connections.push_back(line);
    }
}

PetriObject::~PetriObject() {
    for (auto connection : m_connections) {
        //connection->disconnect(net());
    }
}

QVariant PetriObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        updateConnections();
        return value;
    } else if (change == ItemSceneChange) {
        auto scene = value.value<GraphicScene*>();
        if (scene) {
            scene->registerItem(this);
            this->onAddToScene(scene);
        } else {
            dynamic_cast<GraphicScene*>(this->scene())->unregisterItem(this);
            this->onRemoveFromScene();
        }
    } else if (change == ItemPositionChange && scene()) {
        auto newPosition = value.toPointF();
        if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
            auto graphicScene = qobject_cast<GraphicScene*>(scene());
            qreal gridSize = 50.;
            qreal xV = round(newPosition.x() / gridSize) * gridSize;
            qreal yV = round(newPosition.y() / gridSize) * gridSize;

            return QPointF(xV, yV);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void PetriObject::onAddToScene(GraphicScene *) {
    updateLabelPosition();
}

uint64_t PetriObject::index() const {
    return vertex()->index().id;
}

void PetriObject::connectTo(ffi::PetriNet *net, PetriObject *other) const {
    net->connect(vertex(), other->vertex());
}

ffi::Vertex *PetriObject::vertex() const {
    return m_net->getVertex(m_vertex);
}

void PetriObject::updateLabelPosition() {
    qDebug() << "Here";
    QRectF labelRect = m_labelItem->boundingRect();
    qDebug() << "Here 2";
    //int w = labelRect.width();
    //int h = labelRect.height();

    QRectF rect = boundingRect();
    m_labelItem->setPos(rect.topLeft() - QPointF(labelRect.width(), 0));
}

void PetriObject::updateConnections() {
    for (auto connection : m_connections) {
        connection->updateConnection();
    }
}

void PetriObject::labelChanged() {
    this->vertex()->set_name(m_labelItem->document()->toRawText().toLocal8Bit().data());
    updateLabelPosition();
}

void PetriObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    this->m_labelItem->setFocus(Qt::MouseFocusReason);
    QGraphicsItem::mouseDoubleClickEvent(event);
}

QString PetriObject::label() const {
    return m_labelItem->document()->toRawText();
}

ffi::VertexIndex PetriObject::vertexIndex() const {
    return m_vertex;
}

GraphicScene *PetriObject::graphicScene() const {
    return qobject_cast<GraphicScene*>(scene());
}
