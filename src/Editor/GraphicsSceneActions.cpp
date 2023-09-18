//
// Created by darkp on 17.09.2023.
//

#include "GraphicsSceneActions.h"
#include "GraphicsScene.h"
#include <QUndoStack>

GraphicsSceneActions::GraphicsSceneActions(GraphicsScene *scene) {
    m_undoAction = scene->undoStack()->createUndoAction(scene, tr("&Undo"));
    m_undoAction->setShortcut(QKeySequence::Undo);

    m_redoAction = scene->undoStack()->createRedoAction(scene, tr("&Redo"));
    m_redoAction->setShortcut(QKeySequence::Redo);
}