#include "position.h"
#include "transition.h"
#include "../GraphicScene.h"
#include <QFontMetrics>
#include "../../ffi/simulation.h"

Position::Position(const QPointF& origin, ffi::PetriNet* net, ffi::VertexIndex position, QGraphicsItem* parent) : PetriObject(net, position, parent) {
    this->setPos(origin);
}

Position::Position(const QPointF &origin,
                   ffi::PetriNet* net,
                   ffi::VertexIndex position,
                   PositionState* state,
                   QGraphicsItem *parent)
    : PetriObject(net, position, parent)
    , m_state(state)
{
    this->setPos(origin);
}

QRectF Position::boundingRect() const {
    return shape().boundingRect().adjusted(-2, -20, 2, 20);
}

void Position::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    qreal penWidth = 2;

    auto scene = graphicScene();
    auto simulation = scene->simulation();
    auto rect = this->boundingRect();

    painter->save();
    painter->setPen(QPen(isSelected() ? Qt::green : painter->pen().color(), penWidth));
    painter->drawEllipse(rect.center(), radius - penWidth / 2., radius - penWidth / 2.);

    auto name = QString("P%1%2")
            .arg(this->index())
            .arg(this->vertex()->parent() == 0 ? "" : QString(".%1").arg(this->vertex()->parent()));

    QFontMetricsF metric = QFontMetricsF(painter->font());
    QSizeF nameSize = metric.size(0, name);
    painter->drawText(rect.center().x() - nameSize.width() / 2
                      , rect.center().y() - radius - 5
                      , name);

    int markers = 0;
    if (simulation) {
        markers = simulation->markers(this->vertexIndex());
    } else {
        markers = this->markers();
    }

    if (markers == 1) {
        painter->save();
        painter->setBrush(QBrush(Qt::black));
        painter->drawEllipse(boundingRect().center(), 3., 3.);
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

QPointF Position::connectionPos(PetriObject* to, bool reverse) {

    qreal angle = this->angleBetween(to->scenePos());

    qreal x = scenePos().x();
    qreal y = scenePos().y();

    angle = angle - qDegreesToRadians(90);
    qreal xPosy = (x + qCos(angle) * radius);
    qreal yPosy = (y + qSin(angle) * radius);

    return {xPosy, yPosy};
}

int Position::markers() const {
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

void Position::onAddToScene(GraphicScene* scene) {
    if (m_state) {
        auto net = scene->net();
        auto position = net->add_position_with(m_vertex.id);
        position->set_markers(m_state->markers);
        position->set_parent({ffi::VertexType::Position, (ffi::usize)m_state->parent});
    }

    PetriObject::onAddToScene(scene);
}

void Position::onRemoveFromScene() {
    auto scene = dynamic_cast<GraphicScene*>(this->scene());
    auto net = scene->net();
    auto vertex = net->getVertex(m_vertex);

    m_state = new PositionState();
    m_state->markers = (int)vertex->markers();
    m_state->parent = (int)vertex->parent();

    net->remove_position(vertex);
}

