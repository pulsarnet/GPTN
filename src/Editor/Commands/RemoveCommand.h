//
// Created by darkp on 16.10.2022.
//

#ifndef FFI_RUST_REMOVECOMMAND_H
#define FFI_RUST_REMOVECOMMAND_H

#include <QUndoCommand>

class GraphicsScene;
class PetriObject;
class ArrowLine;

class RemoveCommand : public QUndoCommand {

public:
    explicit RemoveCommand(PetriObject* item, GraphicsScene* scene, QUndoCommand* parent = nullptr);

    void redo() override;

    void undo() override;

private:

    GraphicsScene* m_scene;
    PetriObject* m_item;
    QList<ArrowLine*> m_connections;

};


#endif //FFI_RUST_REMOVECOMMAND_H
