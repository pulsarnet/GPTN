//
// Created by darkp on 15.10.2022.
//

#include "AddCommand.h"
#include "../GraphicsScene.h"
#include "../elements/PetriObject.h"
#include "../elements/Position.h"
#include "../elements/Transition.h"

AddCommand::AddCommand(PetriObject* item, GraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_scene(scene)
    , m_item(item)
{ }

void AddCommand::redo() {
    m_scene->addItem(m_item);
}

void AddCommand::undo() {
    m_scene->removeItem(m_item);
}

AddCommand::~AddCommand() {
    // Удалим item так как он уже не нужен
    delete m_item;
}
