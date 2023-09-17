//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_MARKCOMMAND_H
#define FFI_RUST_MARKCOMMAND_H
#include "../../Core/FFI/rust.h"

#include <QUndoCommand>

class GraphicsScene;
class PetriObject;

class MarkCommand : public QUndoCommand {

public:

    explicit MarkCommand(PetriObject* item, bool add, GraphicsScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:

    PetriObject* m_item;
    bool m_add;
    GraphicsScene* m_scene;
};


#endif //FFI_RUST_MARKCOMMAND_H
