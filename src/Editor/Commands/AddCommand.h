#ifndef FFI_RUST_ADDCOMMAND_H
#define FFI_RUST_ADDCOMMAND_H

#include <QUndoCommand>

class PetriObject;
class GraphicsScene;


class AddCommand final : public QUndoCommand {

public:

    explicit AddCommand(PetriObject* item,
                        GraphicsScene* scene,
                        QUndoCommand* parent = nullptr);

    void undo() override;
    void redo() override;

    ~AddCommand() override;

private:

    GraphicsScene* m_scene;
    PetriObject* m_item;
};

#endif //FFI_RUST_ADDCOMMAND_H
