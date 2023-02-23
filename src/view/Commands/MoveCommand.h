//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_MOVECOMMAND_H
#define FFI_RUST_MOVECOMMAND_H

#include <QUndoCommand>
#include <QTransform>

class PetriObject;

struct MoveCommandData {
    PetriObject* item = nullptr;
    QPointF oldPos;
    QPointF newPos;
};

class MoveCommand : public QUndoCommand {

public:

    explicit MoveCommand(QList<MoveCommandData>&& items, QUndoCommand *parent = nullptr);

    void undo() override;
    void redo() override;

private:

    QList<MoveCommandData> m_items;

};


#endif //FFI_RUST_MOVECOMMAND_H
