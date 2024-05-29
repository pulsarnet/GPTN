#include "ConnectCommand.h"
#include "../GraphicsScene.h"
#include "../elements/PetriObject.h"
#include "../elements/Edge.h"
#include <ptn/net.h>

#include "../Elements/DirectedArc.h"
#include "../Elements/InhibitorArc.h"

ConnectCommand::ConnectCommand(Edge* connection, ConnectionType type, GraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_connection(connection)
    , m_type(type)
{

}

ConnectCommand::ConnectCommand(Edge* connection,
                               ConnectionType type,
                               int newWeight,
                               int oldWeight,
                               bool reverse,
                               GraphicsScene *scene,
                               QUndoCommand *parent)
        : QUndoCommand(parent)
        , m_scene(scene)
        , m_connection(connection)
        , m_type(type)
        , m_newWeight(newWeight)
        , m_oldWeight(oldWeight)
        , m_reverse(reverse)
{

}

void ConnectCommand::redo() {
    switch (m_type) {
        case Connect:
            m_scene->addItem(m_connection);
            break;
        case Disconnect:
            m_scene->removeItem(m_connection);
            break;
        case SetWeight:
            m_connection->setWeight(m_newWeight, m_reverse);
            break;
        case Bidirectional:
            m_connection->setBidirectional(true);
            break;
    }
}

void ConnectCommand::undo() {
    switch (m_type) {
        case Connect:
            m_scene->removeItem(m_connection);
            break;
        case Disconnect:
            m_scene->addItem(m_connection);
            break;
        case SetWeight:
            m_connection->setWeight(m_oldWeight, m_reverse);
            break;
        case Bidirectional:
            m_connection->setBidirectional(false);
            break;
    }
}

ConnectCommand *ConnectCommand::directed(PetriObject *from, PetriObject *to, GraphicsScene *scene) {
    return new ConnectCommand(new DirectedArc(from, to), Connect, scene);
}

ConnectCommand *ConnectCommand::inhibitor(Position *place, Transition *transition, GraphicsScene *scene) {
    return new ConnectCommand(new InhibitorArc(place, transition), Connect, scene);
}

ConnectCommand *ConnectCommand::disconnect(Edge *connection, GraphicsScene *scene) {
    return new ConnectCommand(connection, Disconnect, scene);
}

ConnectCommand *ConnectCommand::setBidirectional(Edge *connection, GraphicsScene *scene) {
    return new ConnectCommand(connection, Bidirectional, scene);
}

ConnectCommand *ConnectCommand::setWeight(Edge *connection, int newWeight, bool reverse, GraphicsScene *scene) {
    return new ConnectCommand(connection,
                              SetWeight,
                              newWeight,
                              (int)connection->weight(reverse),
                              reverse,
                              scene);
}