//
// Created by darkp on 15.10.2022.
//

#include "MoveCommand.h"

#include <utility>
#include "../elements/PetriObject.h"

MoveCommand::MoveCommand(QList<MoveCommandData>&& items, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_items(std::move(items))
{

}

void MoveCommand::redo() {
    for (auto& data : m_items) {
        data.item->setPos(data.newPos);
        data.item->updateConnections();
    }
}

void MoveCommand::undo() {
    for (auto& data : m_items) {
        data.item->setPos(data.oldPos);
        data.item->updateConnections();
    }
}