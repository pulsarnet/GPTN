//
// Created by nmuravev on 12/14/2021.
//

#include "arrow_line.h"
#include "petri_object.h"
#include "transition.h"
#include "../GraphicsScene.h"
#include <QMatrix4x4>

ArrowLine::ArrowLine(PetriObject* from, const QLineF &line, QGraphicsItem* parent)
    : QGraphicsLineItem(line, parent)
    , m_from(from)
{
    setFlags(ItemIsSelectable);
    setAcceptHoverEvents(true);
}

ArrowLine::ArrowLine(PetriObject *from, PetriObject *to, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_from(from)
{
    setTo(to);
    setAcceptHoverEvents(true);
}

ArrowLine::ArrowLine(PetriObject *from, PetriObject *to, ArrowLine::ConnectionState *state, QGraphicsItem *parent)
    : QGraphicsLineItem(parent)
    , m_from(from)
    , m_state(state)
{
    setTo(to);
    setAcceptHoverEvents(true);
}

QRectF ArrowLine::boundingRect() const {
    QRectF rect;

    if (m_to) {
        rect = m_shape.boundingRect().adjusted(-15., -15, 15, 15);
    }
    else {
        rect = QGraphicsLineItem::boundingRect();
        rect.setX(rect.x() - 6.);
        rect.setY(rect.y() - 6.);
        rect.setWidth(rect.width() + 12);
        rect.setHeight(rect.height() + 12);
    }

    return rect;
}

void ArrowLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
    painter->drawPolygon(m_arrow1);

    if (m_bidirectional) {
        painter->drawPolygon(m_arrow2);
    }
    painter->restore();

    if (!m_text.isEmpty()) {
        double angle = std::atan2(-line().dy(), line().dx()) * 180/M_PI;
        painter->save();
        painter->translate(line().center().x(), line().center().y());
        painter->rotate(-angle);
        painter->setBrush(QBrush(Qt::white));
        QFontMetricsF metrics(painter->font());
        QSizeF size = metrics.size(0, m_text);
        painter->drawText(-size.width() / 2, -2, m_text);
        painter->rotate(+angle);
        painter->restore();
    }
}

bool ArrowLine::setTo(PetriObject *to) {
    if (this->m_from->allowConnection(to)) {
        if (m_to) m_to->removeConnectionLine(this);
        m_to = to;

        m_from->addConnectionLine(this);
        m_to->addConnectionLine(this);

        return true;
    }

    return false;
}

void ArrowLine::setBidirectional(bool b) {
    m_bidirectional = b;
    updateConnection();
}

ffi::Connection *ArrowLine::netItem(bool reverse) {
    auto scene = dynamic_cast<GraphicsScene*>(this->scene());
    if (reverse)
        return scene->net()->get_connection(m_to->vertex(), m_from->vertex());
    else
        return scene->net()->get_connection(m_from->vertex(), m_to->vertex());
}

void ArrowLine::updateConnection() {

    if (from() && to()) {
        auto point1 = from()->connectionPos(to());
        auto point2 = to()->connectionPos(from());

        setLine(QLineF(point1, point2));
    }

    QLineF line1(this->line().p2(), this->line().p1());
    QLineF line2(this->line().p1(), this->line().p2());

    m_arrow1 = ArrowLine::arrow(line1);
    m_arrow2 = ArrowLine::arrow(line2);

    QPainterPath curve;
    {
        QPointF arrowLineCenter = QLineF(m_arrow1.data()[1], m_arrow1.data()[2]).center();
        QPointF endPoint = line1.p1();
        endPoint.setX(arrowLineCenter.x());
        endPoint.setY(arrowLineCenter.y());
        curve.moveTo(endPoint);
    }

    if (m_bidirectional) {
        QPointF arrowLineCenter = QLineF(m_arrow2.data()[1], m_arrow2.data()[2]).center();
        QPointF beginPoint = line1.p2();
        beginPoint.setX(arrowLineCenter.x());
        beginPoint.setY(arrowLineCenter.y());
        curve.lineTo(beginPoint);
    } else {
        curve.lineTo(line1.p2());
    }


    if (to()) {
        auto net = dynamic_cast<GraphicsScene*>(this->scene())->net();

        m_text.clear();
        if (m_bidirectional) {
            m_text = QString("<- %1 | %2 ->")
                    .arg(net->connection_weight(to()->vertex(), from()->vertex()))
                    .arg(net->connection_weight(from()->vertex(), to()->vertex()));
        } else if (auto weight = net->connection_weight(from()->vertex(), to()->vertex()); weight > 1) {
            m_text = QString("%1").arg(weight);
        }
    }

    m_shape = curve;
    update();
}

QPolygonF ArrowLine::arrow(QLineF line) {
    static const size_t arrowSize = 10;
    double angle = std::atan2(-line.dy(), line.dx());
    QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI / 3.) * arrowSize);
    QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3.) * arrowSize,
                                          cos(angle + M_PI - M_PI / 3.) * arrowSize);
    QPolygonF arrow;
    arrow << line.p1() << arrowP1 << arrowP2;
    return arrow;
}

QVariant ArrowLine::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
    if (change == ItemSceneChange) {
        if (!m_from || !m_to) return value;

        auto scene = value.value<GraphicsScene*>();
        if (scene) {
            scene->registerItem(this);
            if (m_createInNet)
                this->onAddToScene(scene);
        } else {
            dynamic_cast<GraphicsScene*>(this->scene())->unregisterItem(this);
            this->onRemoveFromScene();
        }
    } else if (change == ItemSceneHasChanged) {
        if (scene()) updateConnection();
    }

    return QGraphicsItem::itemChange(change, value);
}

void ArrowLine::onAddToScene(GraphicsScene *scene) {
    auto net = scene->net();

    net->connect(m_from->vertex(), m_to->vertex());

    if (m_state)
        net->get_connection(m_from->vertex(), m_to->vertex())->setWeight(m_state->weight);

    if (m_bidirectional) {
        net->connect(m_to->vertex(), m_from->vertex());

        if (m_state)
            net->get_connection(m_to->vertex(), m_from->vertex())->setWeight(m_state->weight);
    }

    delete m_state;
    m_state = nullptr;
}

void ArrowLine::onRemoveFromScene() {
    Q_ASSERT(!m_state);

    // Save state
    m_state = new ConnectionState();
    m_state->weight = (int)netItem()->weight();

    // Remove from objects
    m_from->removeConnectionLine(this);
    m_to->removeConnectionLine(this);

    // Remove from net
    auto net = dynamic_cast<GraphicsScene*>(this->scene())->net();
    net->remove_connection(m_from->vertex(), m_to->vertex());
    if (m_bidirectional)
        net->remove_connection(m_to->vertex(), m_from->vertex());
}

void ArrowLine::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    QGraphicsItem::hoverEnterEvent(event);
}
