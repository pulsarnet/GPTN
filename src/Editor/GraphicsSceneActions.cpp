//
// Created by darkp on 17.09.2023.
//

#include "GraphicsSceneActions.h"
#include "GraphicsScene.h"
#include <QUndoStack>
#include <QMenu>

GraphicsSceneActions::GraphicsSceneActions(GraphicsScene *scene) {
    m_undoAction = scene->undoStack()->createUndoAction(scene, tr("&Undo"));
    m_undoAction->setShortcut(QKeySequence::Undo);

    m_redoAction = scene->undoStack()->createRedoAction(scene, tr("&Redo"));
    m_redoAction->setShortcut(QKeySequence::Redo);

    bool hasSelection = !scene->selectedItems().empty();
    m_hAlignment = new QAction("Horizontal alignment");
    m_hAlignment->setEnabled(hasSelection);
    connect(m_hAlignment, &QAction::triggered, scene, &GraphicsScene::slotHorizontalAlignment);

    m_vAlignment = new QAction("Vertical alignment");
    m_vAlignment->setEnabled(hasSelection);
    connect(m_vAlignment, &QAction::triggered, scene, &GraphicsScene::slotVerticalAlignment);

    m_graphVizMenu = new QMenu(tr("Graphviz algorithms"));
    m_graphVizMenu->addAction(tr("dot"),[scene]() { scene->dotVisualization((char*)"dot"); });
    m_graphVizMenu->addAction(tr("neato"),[scene]() { scene->dotVisualization((char*)"neato"); });
    m_graphVizMenu->addAction(tr("twopi"),[scene]() { scene->dotVisualization((char*)"twopi"); });
    m_graphVizMenu->addAction(tr("circo"),[scene]() { scene->dotVisualization((char*)"circo"); });
    m_graphVizMenu->addAction(tr("fdp"),[scene]() { scene->dotVisualization((char*)"fdp"); });
    m_graphVizMenu->addAction(tr("dot"),[scene]() { scene->dotVisualization((char*)"sfdp"); });
}