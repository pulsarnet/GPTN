//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_MOVECOMMAND_H
#define FFI_RUST_MOVECOMMAND_H

#include <QUndoCommand>
#include <QTransform>

class PetriObject;

class MoveCommand : public QUndoCommand {

public:

    explicit MoveCommand(PetriObject *item, const QPointF& oldPos, const QPointF& newPos, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:

    PetriObject* m_item;
    QPointF m_oldPos;
    QPointF m_newPos;

};


#endif //FFI_RUST_MOVECOMMAND_H
