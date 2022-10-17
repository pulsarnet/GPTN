//
// Created by darkp on 15.10.2022.
//

#include "ConnectCommand.h"
#include "../GraphicScene.h"
#include "../elements/petri_object.h"
#include "../elements/arrow_line.h"

ConnectCommand::ConnectCommand(ArrowLine* connection, ConnectionType type, GraphicScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_connection(connection)
    , m_type(type)
{

}

ConnectCommand::ConnectCommand(ArrowLine* connection,
                               ConnectionType type,
                               int newWeight,
                               int oldWeight,
                               bool reverse,
                               GraphicScene *scene,
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
            m_scene->setConnectionWeight(m_connection, m_newWeight, m_reverse);
            break;
        case Bidirectional:
            // Добавить обратное соединение в объект net, для правильного отображения
            m_scene->net()->connect(m_connection->to()->vertex(), m_connection->from()->vertex());
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
            m_scene->setConnectionWeight(m_connection, m_oldWeight, m_reverse);
            break;
        case Bidirectional:
            // Сначала удалить обратное соединение из объекта net, для правильного отображения
            m_scene->net()->remove_connection(m_connection->to()->vertex(), m_connection->from()->vertex());
            m_connection->setBidirectional(false);
            break;
    }
}

ConnectCommand *ConnectCommand::connect(PetriObject *from, PetriObject *to, GraphicScene *scene) {
    return new ConnectCommand(new ArrowLine(from, to), Connect, scene);
}

ConnectCommand *ConnectCommand::disconnect(ArrowLine *connection, GraphicScene *scene) {
    return new ConnectCommand(connection, Disconnect, scene);
}

ConnectCommand *ConnectCommand::setBidirectional(ArrowLine *connection, GraphicScene *scene) {
    return new ConnectCommand(connection, Bidirectional, scene);
}

ConnectCommand *ConnectCommand::setWeight(ArrowLine *connection, int newWeight, bool reverse, GraphicScene *scene) {
    return new ConnectCommand(connection,
                              SetWeight,
                              newWeight,
                              (int)connection->netItem(reverse)->weight(),
                              reverse,
                              scene);
}