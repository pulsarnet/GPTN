//
// Created by Николай Муравьев on 10.01.2022.
//

#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include "position.h"
#include "transition.h"
#include "../GraphicsScene.h"
#include "../../Core/FFI/simulation.h"

Transition::Transition(const QPointF& origin,
                       ffi::PetriNet* net,
                       ffi::VertexIndex transition,
                       QGraphicsItem *parent)
    : PetriObject(net, transition, parent)
    , m_origin(origin)
{
    this->setPos(m_origin);
}

Transition::Transition(const QPointF& origin,
                       ffi::PetriNet* net,
                       ffi::VertexIndex transition,
                       TransitionState* state,
                       QGraphicsItem *parent)
        : PetriObject(net, transition, parent)
        , m_origin(origin)
        , m_state(state)
{
    this->setPos(m_origin);
}


QRectF Transition::boundingRect() const {
    return !m_rotated ? QRectF(-10, -30, 20, 60) : QRectF(-30, -10, 60, 20);

}

void Transition::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipRect(boundingRect());

    painter->save();

    painter->setPen(isSelected() ? QPen(Qt::red) : Qt::NoPen);

    // scene
    auto scene = graphicScene();
    auto simulation = scene->simulation();
    if (simulation && simulation->isFired(this->vertexIndex())) {
        painter->setBrush(QBrush(Qt::green));
    } else {
        painter->setBrush(QBrush(Qt::black));
    }

    auto rect = boundingRect();
    rect.setSize(QSize(rect.size().width(), rect.size().height()));
    rect.setX(rect.x());
    rect.setY(rect.y());
    painter->drawRect(rect);

    painter->restore();
}

QPointF Transition::center() {
    return this->scenePos();
}

QPointF getIntersection(qreal dx, qreal dy, qreal cx, qreal cy, qreal width, qreal height) {
    if (qAbs(dy / dx) < height / width) {
        return QPointF(cx + (dx > 0 ? width : -width), cy + dy * width / qAbs(dx));
    }
    return QPointF(cx + dx * height / qAbs(dy), cy + (dy > 0 ? height : -height));
}

QPointF Transition::connectionPos(PetriObject* to) {
    qreal w = QGraphicsItem::sceneBoundingRect().width() / 2.;
    qreal h = QGraphicsItem::sceneBoundingRect().height() / 2.;

    qreal xPosy = to->scenePos().x();
    qreal yPosy = to->scenePos().y();

    qreal dx = xPosy - center().x();
    qreal dy = yPosy - center().y();

    qreal cx = center().x();
    qreal cy = center().y();


    auto intersection = dx == 0. && dy == 0. ? to->scenePos() : getIntersection(dx, dy, cx, cy,w, h);

    return intersection;
}

QString Transition::name() const {
    return QString("t%1").arg(index());
}

void Transition::onAddToScene(GraphicsScene* scene) {
    auto net = scene->net();

    if (m_state) {
        auto transition = net->add_transition_with(m_vertex.id);
        transition->set_parent({ffi::VertexType::Transition, (ffi::usize)m_state->parent});
    }

    PetriObject::onAddToScene(scene);
}

void Transition::onRemoveFromScene() {
    auto scene = dynamic_cast<GraphicsScene*>(this->scene());
    auto net = scene->net();
    auto vertex = net->getVertex(m_vertex);

    m_state = new TransitionState();
    m_state->parent = (int)vertex->parent();

    net->remove_transition(vertex);
}

QVariant Transition::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if (change == ItemRotationChange) {
        m_rotated = !m_rotated;
        updateLabelPosition();
        return 0;
    }
    return PetriObject::itemChange(change, value);
}
