//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_ADDCOMMAND_H
#define FFI_RUST_ADDCOMMAND_H

#include <QUndoCommand>
#include <QPointF>
#include "../../ffi/rust.h"

class PetriObject;
class GraphicScene;

namespace ffi {
    struct PetriNet;
}

class AddCommand : public QUndoCommand {

public:

    explicit AddCommand(PetriObject* item,
                        GraphicScene* scene,
                        QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

    ~AddCommand();

private:

    GraphicScene* m_scene;
    PetriObject* m_item;
};

#endif //FFI_RUST_ADDCOMMAND_H
