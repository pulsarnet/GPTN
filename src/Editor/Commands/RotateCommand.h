#ifndef FFI_RUST_ROTATECOMMAND_H
#define FFI_RUST_ROTATECOMMAND_H

#include <QUndoCommand>

class PetriObject;

class RotateCommand : public QUndoCommand {

public:

    explicit RotateCommand(PetriObject *item, qreal angle, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:

    PetriObject* m_item;
    qreal m_angle;
};


#endif //FFI_RUST_ROTATECOMMAND_H
