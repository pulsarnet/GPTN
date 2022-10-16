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

void ConnectCommand::redo() {
    auto net = m_scene->net();
    if (m_type == Connect) {
        net->connect(m_connection->from()->vertex(), m_connection->to()->vertex());
        m_scene->addItem(m_connection);
    } else if (m_type == Disconnect) {
        net->remove_connection(m_connection->from()->vertex(), m_connection->to()->vertex());
        m_scene->removeItem(m_connection);
    } else if (m_type == IncrementWeight) {
        auto connection = net->get_connection(m_connection->from()->vertex(), m_connection->to()->vertex());
        connection->setWeight(connection->weight() + 1);
    } else if (m_type == DicrementWeight) {
        auto connection = net->get_connection(m_connection->from()->vertex(), m_connection->to()->vertex());
        connection->setWeight(connection->weight() - 1);
    } else if (m_type == Bidirectional) {
        // set bidirectional
        net->connect(m_connection->to()->vertex(), m_connection->from()->vertex());
        m_connection->setBidirectional(true);
    }
}

void ConnectCommand::undo() {
    auto net = m_scene->net();
    if (m_type == Connect) {
        net->remove_connection(m_connection->from()->vertex(), m_connection->to()->vertex());
        m_scene->removeItem(m_connection);
    } else if (m_type == Disconnect) {
        net->connect(m_connection->from()->vertex(), m_connection->to()->vertex());
        m_scene->addItem(m_connection);
    } else if (m_type == IncrementWeight) {
        auto connection = net->get_connection(m_connection->from()->vertex(), m_connection->to()->vertex());
        connection->setWeight(connection->weight() - 1);
    } else if (m_type == DicrementWeight) {
        auto connection = net->get_connection(m_connection->from()->vertex(), m_connection->to()->vertex());
        connection->setWeight(connection->weight() + 1);
    } else if (m_type == Bidirectional) {
        // set directional
        net->remove_connection(m_connection->to()->vertex(), m_connection->from()->vertex());
        m_connection->setBidirectional(false);
    }
}
