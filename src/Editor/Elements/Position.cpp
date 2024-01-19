#include "Position.h"
#include "Transition.h"
#include "../GraphicsScene.h"
#include <ptn/simulation.h>
#include <QFontMetrics>

Position::Position(const QPointF& origin, net::PetriNet* net, vertex::VertexIndex position, QGraphicsItem* parent)
    : PetriObject(net, position, parent)
{
    this->setPos(origin);
}

Position::Position(const QPointF &origin,
                   net::PetriNet* net,
                   vertex::VertexIndex position,
                   PositionState* state,
                   QGraphicsItem *parent)
    : PetriObject(net, position, parent)
    , m_state(state)
{
    this->setPos(origin);
}

QRectF Position::boundingRect() const {
    return shape().boundingRect().normalized();
}

void Position::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)
    
    qreal penWidth = 2;

    auto scene = graphicScene();
    auto simulation = scene->simulation();
    auto rect = this->boundingRect();

    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(isSelected() ? Qt::green : painter->pen().color(), penWidth));
    painter->drawEllipse(rect.center(), radius - penWidth / 2., radius - penWidth / 2.);

    size_t markers = simulation ? simulation->markers_at(this->vertexIndex()) : this->markers();
    if (markers == 1) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));
        painter->drawEllipse(rect.center(), 3., 3.);
        painter->restore();
    } else if (markers == 2) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (markers == 3) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() + 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() - 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (markers == 4) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 9.);
        rect.setY(rect.y() + 9.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x());
        rect.setY(rect.y() - 18.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 18.);
        rect.setY(rect.y());
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (markers == 5) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));

        rect.setX(rect.x() - 12.);
        rect.setY(rect.y() + 12.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 24.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x());
        rect.setY(rect.y() - 24.);
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() - 24.);
        rect.setY(rect.y());
        painter->drawEllipse(rect.center(), 3., 3.);

        rect.setX(rect.x() + 12.);
        rect.setY(rect.y() + 12.);
        painter->drawEllipse(rect.center(), 3., 3.);

        painter->restore();
    } else if (markers > 5) {
        painter->drawText(rect, Qt::AlignCenter, QString("%1").arg(markers));
    }

    painter->restore();
}

QPointF Position::center() {
    return scenePos();
}

QPointF Position::connectionPos(PetriObject* to) {

    qreal angle = this->angleBetween(to->scenePos());

    qreal x = scenePos().x();
    qreal y = scenePos().y();

    angle = angle - qDegreesToRadians(90);
    qreal xPosy = (x + qCos(angle) * radius);
    qreal yPosy = (y + qSin(angle) * radius);

    return {xPosy, yPosy};
}

size_t Position::markers() const {
    return vertex()->markers();
}

void Position::setMarkers(int markers) {
    vertex()->set_markers(markers);
}

QString Position::name() const {
    return QString("p%1").arg(index());
}

QPainterPath Position::shape() const {
    QPainterPath path;
    path.addEllipse({0, 0}, radius, radius);
    return path;
}

void Position::onAddToScene(GraphicsScene* scene) {
    if (m_state) {
        auto net = scene->net();
        auto position = net->insert_position(m_vertex.id, m_state->parent);
        position->set_markers(m_state->markers);
    }

    PetriObject::onAddToScene(scene);
}

void Position::onRemoveFromScene() {
    auto scene = dynamic_cast<GraphicsScene*>(this->scene());
    auto net = scene->net();
    auto vertex = net->vertex(m_vertex);

    m_state = new PositionState();
    m_state->markers = (int)vertex->markers();
    m_state->parent = (int)vertex->parent().id;

    net->remove(vertex->index());
}

