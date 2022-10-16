//
// Created by darkp on 15.10.2022.
//

#include "MoveCommand.h"
#include "../elements/petri_object.h"

MoveCommand::MoveCommand(PetriObject *item, const QPointF& oldPos, const QPointF& newPos, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_item(item)
    , m_oldPos(oldPos)
    , m_newPos(newPos)
{

}

void MoveCommand::redo() {
    m_item->setPos(m_newPos);
    m_item->updateConnections();
}

void MoveCommand::undo() {
    m_item->setPos(m_oldPos);
    m_item->updateConnections();
}