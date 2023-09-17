//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_CONNECTCOMMAND_H
#define FFI_RUST_CONNECTCOMMAND_H

#include <QUndoCommand>

class GraphicsScene;
class ArrowLine;
class PetriObject;

class ConnectCommand : public QUndoCommand {

    enum ConnectionType {
        Connect,
        Disconnect,
        SetWeight,
        Bidirectional
    };

    explicit ConnectCommand(ArrowLine* connection,
                            ConnectionType type,
                            GraphicsScene* scene,
                            QUndoCommand* parent = nullptr);

    explicit ConnectCommand(ArrowLine* connection,
                            ConnectionType type,
                            int newWeight,
                            int oldWeight,
                            bool reverse,
                            GraphicsScene* scene,
                            QUndoCommand* parent = nullptr);

public:

    static ConnectCommand* connect(PetriObject* from, PetriObject* to, GraphicsScene* scene);
    static ConnectCommand* disconnect(ArrowLine* connection, GraphicsScene* scene);
    static ConnectCommand* setWeight(ArrowLine* connection, int newWeight, bool reverse, GraphicsScene* scene);
    static ConnectCommand* setBidirectional(ArrowLine* connection, GraphicsScene* scene);

    void redo() override;
    void undo() override;

private:

    GraphicsScene* m_scene;
    ArrowLine* m_connection;
    ConnectionType m_type;
    int m_newWeight;
    int m_oldWeight;
    bool m_reverse;
};

#endif //FFI_RUST_CONNECTCOMMAND_H
