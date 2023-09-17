//
// Created by darkp on 15.10.2022.
//

#include "RotateCommand.h"
#include "../elements/petri_object.h"

RotateCommand::RotateCommand(PetriObject *item, qreal angle, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_item(item)
    , m_angle(angle)
{

}

void RotateCommand::redo() {
    m_item->setRotation(m_item->rotation() + m_angle);
    m_item->updateConnections();
}

void RotateCommand::undo() {
    m_item->setRotation(m_item->rotation() - m_angle);
    m_item->updateConnections();
}
