#include "Edge.h"
#include "PetriObject.h"
#include "../GraphicsScene.h"
#include <QMatrix4x4>
#include <ptn/net.h>

Edge::Edge(PetriObject* from, const QLineF &line, QGraphicsItem* parent)
    : QGraphicsLineItem(line, parent)
    , m_from(from)
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

Edge::Edge(PetriObject *from, PetriObject *to, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_from(from)
    , m_to(to)
{
    setAcceptHoverEvents(true);
}

QRectF Edge::boundingRect() const {
    QRectF rect;

    if (m_to) {
        rect = m_shape.boundingRect().adjusted(-15., -15, 15, 15);
    } else {
        rect = QGraphicsLineItem::boundingRect();
        rect.setX(rect.x() - 6.);
        rect.setY(rect.y() - 6.);
        rect.setWidth(rect.width() + 12);
        rect.setHeight(rect.height() + 12);
    }

    return rect;
}

void Edge::setDestination(PetriObject *to) {
    if (!allowDestination(to) || m_to == to)
        return;

    // unregister connection line from destination
    if (m_to) m_to->removeConnectionLine(this);
    // set new destination
    m_to = to;

    // add line to from and to
    m_from->addConnectionLine(this);
    m_to->addConnectionLine(this);

    edgeChange(EdgeDestinationHasChanged, QVariant::fromValue(to));
}

void Edge::setBidirectional(bool b) {
    if (!(m_flags & EdgeCanBidirectional)) {
        b = false;
    }

    if (m_bidirectional == b)
        return;

    const QVariant newBidirectionalVariant(edgeChange(EdgeBidirectionalChanged, quint32(b)));
    bool newBidirectional = newBidirectionalVariant.toBool();
    if (m_bidirectional == newBidirectional)
        return;

    m_bidirectional = newBidirectional;
    updateConnection();

    edgeChange(EdgeBidirectionalHasChanged, quint32(m_bidirectional));
}

uint32_t Edge::weight(bool reverse) const {
    return m_weight[size_t(reverse)];
}

void Edge::setWeight(uint32_t weight, bool reverse) {
    if (!(m_flags & EdgeHasWeight))
        return;

    uint32_t current = Edge::weight(reverse);
    if (weight == current)
        return;

    const QVariant newWeightVariant(edgeChange(reverse ? EdgeWeightReverseChanged : EdgeWeightChanged, weight));
    uint32_t newWeight = newWeightVariant.toUInt();
    if (current == newWeight)
        return;

    m_weight[size_t(reverse)] = newWeight;
    updateConnection();

    edgeChange(reverse ? EdgeWeightReverseHasChanged : EdgeWeightHasChanged, m_weight[size_t(reverse)]);
}

void Edge::updateConnection() {
    if (from() && to()) {
        auto point1 = from()->connectionPos(to());
        auto point2 = to()->connectionPos(from());

        setLine(QLineF(point1, point2));
    }

    QLineF line1(line().p1(), line().p2());
    QLineF line2(line().p2(), line().p1());

    m_nodeShape[0] = nodeShapeAt(line1);
    m_nodeShape[1] = nodeShapeAt(line2);

    QPainterPath curve;
    {
        //QPointF arrowLineCenter = QLineF(m_nodeShape[0].data()[1], m_nodeShape.data()[2]).center();
        QPointF endPoint = line1.p1();
        // endPoint.setX(arrowLineCenter.x());
        // endPoint.setY(arrowLineCenter.y());
        curve.moveTo(endPoint);
    }

    if (m_bidirectional) {
        //QPointF arrowLineCenter = QLineF(m_arrow2.data()[1], m_arrow2.data()[2]).center();
        QPointF beginPoint = line1.p2();
        // beginPoint.setX(arrowLineCenter.x());
        // beginPoint.setY(arrowLineCenter.y());
        curve.lineTo(beginPoint);
    } else {
        curve.lineTo(line1.p2());
    }

    //
    // if (to()) {
    //     auto net = dynamic_cast<GraphicsScene*>(this->scene())->net();
    //
    //     m_text.clear();
    //     if (m_bidirectional) {
    //         m_text = QString("<- %1 | %2 ->")
    //                 .arg(net->directed_arc(to()->vertexIndex(), from()->vertexIndex())->weight())
    //                 .arg(net->directed_arc(from()->vertexIndex(), to()->vertexIndex())->weight());
    //     } else if (auto weight = net->directed_arc(from()->vertexIndex(), to()->vertexIndex())->weight(); weight > 1) {
    //         m_text = QString("%1").arg(weight);
    //     }
    // }
    //
    m_shape = curve;
    update();
}

QVariant Edge::edgeChange(EdgeChange change, const QVariant &value) {
    if (change == EdgeSceneChanged) {
        auto oldScene = dynamic_cast<GraphicsScene*>(scene());
        auto newScene = value.value<GraphicsScene*>();
        if (!oldScene || newScene == oldScene) return value; // scene not changed or null

        m_from->removeConnectionLine(this);
        m_to->removeConnectionLine(this);
        oldScene->unregisterItem(this);
    } else if (change == EdgeSceneHasChanged) {
        auto s = dynamic_cast<GraphicsScene*>(scene());
        if (!s) return value;

        m_from->addConnectionLine(this);
        m_to->addConnectionLine(this);
        s->registerItem(this);

        updateConnection();
    }

    return value;
}


QVariant Edge::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    // TODO: когда идет отвязка от сцены ItemSceneChange надо удалить все что связанно
    // с текущей сценой, а также удалить из scene()->net()
    // Когда задается новая сцена ItemSceneHasChanged, добавить все привязки
    if (!m_blockChange) {
        if (change == ItemSceneChange || change == ItemSceneHasChanged)
            edgeChange(mapToChange(change), value);
    }

    return QGraphicsItem::itemChange(change, value);
}

void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipRect(boundingRect());
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing);

    painter->save();
    painter->setBrush(Qt::NoBrush);
    painter->setPen(Qt::black);
    painter->drawPath(m_shape);

    painter->setBrush(Qt::black);
    painter->setPen(Qt::NoPen);
    painter->drawPath(m_nodeShape[0]);

    if (isBidirectional()) {
        painter->drawPath(m_nodeShape[1]);
    }
    painter->restore();

    // if (!m_text.isEmpty()) {
    //     double angle = std::atan2(-line().dy(), line().dx()) * 180/M_PI;
    //     painter->save();
    //     painter->translate(line().center().x(), line().center().y());
    //     painter->rotate(-angle);
    //     painter->setBrush(QBrush(Qt::white));
    //     QFontMetricsF metrics(painter->font());
    //     QSizeF size = metrics.size(0, m_text);
    //     painter->drawText(-size.width() / 2, -2, m_text);
    //     painter->rotate(+angle);
    //     painter->restore();
    // }
}


void Edge::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsItem::hoverEnterEvent(event);
}

Edge::EdgeChange Edge::mapToChange(GraphicsItemChange change) {
    switch (change) {
        case ItemSceneChange:
            return EdgeSceneChanged;
        case ItemSceneHasChanged:
            return EdgeSceneHasChanged;
        default:
            throw 0;
    }
}

