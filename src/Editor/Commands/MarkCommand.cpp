#include "MarkCommand.h"
#include "../GraphicsScene.h"
#include "../elements/PetriObject.h"
#include <ptn/net.h>

MarkCommand::MarkCommand(PetriObject* item, bool add, GraphicsScene *scene, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_item(item)
    , m_add(add)
    , m_scene(scene)
{

}

void MarkCommand::redo() {
    if (m_add) {
        m_scene->net()->vertex(m_item->vertexIndex())->add_marker();
    } else {
        m_scene->net()->vertex(m_item->vertexIndex())->remove_marker();
    }

    m_item->update();
}

void MarkCommand::undo() {
    if (m_add) {
        m_scene->net()->vertex(m_item->vertexIndex())->remove_marker();
    } else {
        m_scene->net()->vertex(m_item->vertexIndex())->add_marker();
    }

    m_item->update();
}