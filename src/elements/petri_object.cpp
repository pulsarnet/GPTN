//
// Created by Николай Муравьев on 10.01.2022.
//

#include <QTextDocument>
#include <QApplication>
#include "../view/graphic_scene.h"
#include "petri_object.h"
#include "arrow_line.h"
#include "../ffi/rust.h"


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
        for (auto connection : m_connections) {
            connection->updateConnection();
        }

        return value;
    } else if (change == ItemSceneHasChanged) {
        updateLabelPosition();
    } else if (change == ItemPositionChange && scene()) {
        auto newPosition = value.toPointF();
        //qDebug() << QApplication::keyboardModifiers();
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

uint64_t PetriObject::index() const {
    return vertex()->index().id;
}

void PetriObject::connectTo(ffi::PetriNet *net, PetriObject *other) {
    net->connect(vertex(), other->vertex());
}

ffi::Vertex *PetriObject::vertex() const {
    return m_net->getVertex(m_vertex);
}

void PetriObject::updateLabelPosition() {
    int w = m_labelItem->boundingRect().width();
    int h = m_labelItem->boundingRect().height();

    QRectF rect = boundingRect();
    m_labelItem->setPos(-w - rect.width() / 2 - 8, -h);
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

void PetriObject::setLabel(const QString& label) {
    vertex()->set_name(label.toLocal8Bit().data());
    m_labelItem->document()->setPlainText(label);
    updateLabelPosition();
}
