//
// Created by darkp on 15.10.2022.
//

#include "MarkCommand.h"
#include "../GraphicScene.h"
#include "../elements/petri_object.h"

MarkCommand::MarkCommand(PetriObject* item, bool add, GraphicScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_item(item)
    , m_add(add)
    , m_scene(scene)
{

}

void MarkCommand::redo() {
    if (m_add) {
        m_scene->net()->getVertex(m_item->vertexIndex())->add_marker();
    } else {
        m_scene->net()->getVertex(m_item->vertexIndex())->remove_marker();
    }

    m_item->update();
}

void MarkCommand::undo() {
    if (m_add) {
        m_scene->net()->getVertex(m_item->vertexIndex())->remove_marker();
    } else {
        m_scene->net()->getVertex(m_item->vertexIndex())->add_marker();
    }

    m_item->update();
}