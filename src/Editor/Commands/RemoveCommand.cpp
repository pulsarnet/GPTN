//
// Created by darkp on 16.10.2022.
//

#include "RemoveCommand.h"
#include "../GraphicsScene.h"
#include "../elements/PetriObject.h"
#include "../elements/ArrowLine.h"

RemoveCommand::RemoveCommand(PetriObject *item, GraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_item(item)
{

}

void RemoveCommand::redo() {
    // Get connections for item
    auto connections = m_item->connections();
    for (auto& connection : connections) {
//        // Save connection info
//        ConnectionInfo info;
//        info.bidirectional = connection->isBidirectional();
//        if (info.bidirectional)
//            info.weights = {connection->netItem()->weight(), connection->netItem(true)->weight()};
//        else
//            info.weights = {connection->netItem()->weight(), 0};
//
//        m_connections.append({connection, info});

        // Remove connection from scene
        m_connections.append(connection);
        m_scene->removeItem(connection);
    }

    // Remove item
    m_scene->removeItem(m_item);
}

void RemoveCommand::undo() {

    // First: Restore item
    m_scene->addItem(m_item);

    // Restore connections
    for (auto& connection : m_connections) {
        m_scene->addItem(connection);
//        // Restore connection
//        m_scene->addConnection(connection.first);
//
//        // Restore weights
//        connection.first->netItem()->setWeight(connection.second.weights.first);
//        if (connection.second.bidirectional) {
//            m_scene->net()->connect(connection.first->to()->vertex(), connection.first->from()->vertex());
//            connection.first->netItem(true)->setWeight(connection.second.weights.second);
//        }
//
//        connection.first->updateConnection();
    }

    m_connections.clear();
}
