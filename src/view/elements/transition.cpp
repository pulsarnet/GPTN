//
// Created by Николай Муравьев on 10.01.2022.
//

#include <QGraphicsTextItem>
#include <QGraphicsScene>
#include "position.h"
#include "transition.h"
#include "../GraphicScene.h"
#include "../../ffi/simulation.h"

Transition::Transition(const QPointF& origin,
                       ffi::PetriNet* net,
                       ffi::VertexIndex transition,
                       QGraphicsItem *parent)
    : PetriObject(net, transition, parent)
    , m_origin(origin)
{
    this->setPos(m_origin);
    auto name = QString("T%1%2")
            .arg(this->index())
            .arg(this->vertex()->parent() == 0 ? "" : QString(".%1").arg(this->vertex()->parent()));

    m_label = new QGraphicsTextItem(name, this);
    itemChange(ItemRotationHasChanged, {});
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
    auto name = QString("T%1%2")
            .arg(this->index())
            .arg(this->vertex()->parent() == 0 ? "" : QString(".%1").arg(this->vertex()->parent()));

    m_label = new QGraphicsTextItem(name, this);
    itemChange(ItemRotationHasChanged, {});
}


QRectF Transition::boundingRect() const {
    return {-10, -30, 20, 60};
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

QPointF Transition::connectionPos(PetriObject* to, bool reverse) {


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

QVariant Transition::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if (change == ItemRotationHasChanged) {
        setLabelTransformation();
    }
    return PetriObject::itemChange(change, value);
}

void Transition::setLabelTransformation() {
    qreal angle = rotation();

    QFontMetricsF metrics = QFontMetricsF(scene() ? scene()->font() : QFont());
    QSizeF nameSize = metrics.size(0, m_label->toPlainText());
    if (angle != 0) {
        m_label->setRotation(-angle);
        m_label->setPos(-32, boundingRect().y() + boundingRect().height() / 2 + nameSize.width() / 2 + 2);
    } else {
        m_label->setPos(boundingRect().center().x() - nameSize.width() / 2 - 2, -60);
        m_label->setRotation(-angle);
    }
}

void Transition::onAddToScene(GraphicScene* scene) {
    auto net = scene->net();

    if (m_state) {
        auto transition = net->add_transition_with(m_vertex.id);
        transition->set_parent({ffi::VertexType::Transition, (ffi::usize)m_state->parent});
    }
}

void Transition::onRemoveFromScene() {
    auto scene = dynamic_cast<GraphicScene*>(this->scene());
    auto net = scene->net();
    auto vertex = net->getVertex(m_vertex);

    m_state = new TransitionState();
    m_state->parent = (int)vertex->parent();

    net->remove_transition(vertex);
}