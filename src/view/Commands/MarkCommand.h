//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_MARKCOMMAND_H
#define FFI_RUST_MARKCOMMAND_H
#include "../../ffi/rust.h"

#include <QUndoCommand>

class GraphicScene;
class PetriObject;

class MarkCommand : public QUndoCommand {

public:

    explicit MarkCommand(PetriObject* item, bool add, GraphicScene* scene, QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

private:

    PetriObject* m_item;
    bool m_add;
    GraphicScene* m_scene;
};


#endif //FFI_RUST_MARKCOMMAND_H
