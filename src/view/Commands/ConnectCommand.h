//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_CONNECTCOMMAND_H
#define FFI_RUST_CONNECTCOMMAND_H

#include <QUndoCommand>

class GraphicScene;
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
                            GraphicScene* scene,
                            QUndoCommand* parent = nullptr);

    explicit ConnectCommand(ArrowLine* connection,
                            ConnectionType type,
                            int newWeight,
                            int oldWeight,
                            bool reverse,
                            GraphicScene* scene,
                            QUndoCommand* parent = nullptr);

public:

    static ConnectCommand* connect(PetriObject* from, PetriObject* to, GraphicScene* scene);
    static ConnectCommand* disconnect(ArrowLine* connection, GraphicScene* scene);
    static ConnectCommand* setWeight(ArrowLine* connection, int newWeight, bool reverse, GraphicScene* scene);
    static ConnectCommand* setBidirectional(ArrowLine* connection, GraphicScene* scene);

    void redo() override;
    void undo() override;

private:

    GraphicScene* m_scene;
    ArrowLine* m_connection;
    ConnectionType m_type;
    int m_newWeight;
    int m_oldWeight;
    bool m_reverse;
};

#endif //FFI_RUST_CONNECTCOMMAND_H
