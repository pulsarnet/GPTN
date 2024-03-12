#include <QTextDocument>
#include <QApplication>
#include <QGraphicsSceneMouseEvent>
#include <ptn/net.h>
#include "../GraphicsScene.h"
#include "PetriObject.h"
#include "ArrowLine.h"


PetriObject::PetriObject(ptn::net::PetriNet* net, ptn::net::vertex::VertexIndex _vertex, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , m_net(net)
    , m_vertex(_vertex)
{
    m_labelItem = new QGraphicsTextItem(vertex()->label(false), this);
    m_labelItem->setFlags(m_labelItem->flags() & 0);
    m_labelItem->setCacheMode(DeviceCoordinateCache);
    m_labelItem->setTextInteractionFlags(Qt::TextInteractionFlag::TextEditable);
    m_labelItem->setAcceptHoverEvents(false);

    connect(m_labelItem->document(), &QTextDocument::contentsChanged, this, &PetriObject::labelChanged);

    auto name = QString("%1%2%3")
            .arg(this->vertex()->type() == ptn::net::vertex::VertexType::Position ? "P" : "T")
            .arg(this->index())
            .arg(this->vertex()->parent().id == 0 ? "" : QString(".%1").arg(this->vertex()->parent().id));

    m_name = new QGraphicsSimpleTextItem(name, this);
    m_name->setFlags(m_name->flags() & 0);
    m_name->setCacheMode(DeviceCoordinateCache);
    m_name->setAcceptHoverEvents(false);

    setFlags(ItemIsMovable | ItemSendsGeometryChanges | ItemIsSelectable | ItemUsesExtendedStyleOption);
    setCacheMode(DeviceCoordinateCache);
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

QPointF PetriObject::connectionPos(PetriObject* to) {
    Q_UNUSED(to)
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
    // todo а почему?
//    for (auto connection : m_connections) {
//        //connection->disconnect(net());
//    }
}

QVariant PetriObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if (change == ItemPositionHasChanged) {
        updateConnections();
        return value;
    } else if (change == ItemSceneChange) {
        auto scene = value.value<GraphicsScene*>();
        if (scene) {
            scene->registerItem(this);
            this->onAddToScene(scene);
        } else {
            dynamic_cast<GraphicsScene*>(this->scene())->unregisterItem(this);
            this->onRemoveFromScene();
        }
    } else if (change == ItemPositionChange && scene()) {
        auto newPosition = value.toPointF();
        if (QApplication::keyboardModifiers() & Qt::ShiftModifier) {
            qreal gridSize = 50.;
            qreal xV = round(newPosition.x() / gridSize) * gridSize;
            qreal yV = round(newPosition.y() / gridSize) * gridSize;

            return QPointF(xV, yV);
        }
    }

    return QGraphicsItem::itemChange(change, value);
}

void PetriObject::onAddToScene(GraphicsScene *) {
    updateLabelPosition();
}

uint64_t PetriObject::index() const {
    return vertex()->index().id;
}

ptn::net::vertex::Vertex *PetriObject::vertex() const {
    return m_net->vertex(m_vertex);
}

void PetriObject::updateLabelPosition() {
    QRectF labelRect = m_labelItem->boundingRect();
    QRectF nameRect = m_name->boundingRect();
    QRectF objectRect = boundingRect();

    m_labelItem->setPos(objectRect.topLeft() - QPointF(labelRect.width(), labelRect.height()));

    auto offsetX = nameRect.width() / 2.;
    auto offsetY = objectRect.height() / 2. + nameRect.height();
    m_name->setPos(objectRect.center() + QPointF(-offsetX,  -offsetY));
}

void PetriObject::updateConnections() {
    for (auto connection : m_connections) {
        connection->updateConnection();
    }
}

void PetriObject::labelChanged() {
    this->vertex()->set_label(m_labelItem->document()->toRawText().toLocal8Bit().data());
    updateLabelPosition();
}

void PetriObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    this->m_labelItem->setFocus(Qt::MouseFocusReason);
    QGraphicsItem::mouseDoubleClickEvent(event);
}

QString PetriObject::label() const {
    return m_labelItem->document()->toRawText();
}

ptn::net::vertex::VertexIndex PetriObject::vertexIndex() const {
    return m_vertex;
}

GraphicsScene *PetriObject::graphicScene() const {
    return qobject_cast<GraphicsScene*>(scene());
}
