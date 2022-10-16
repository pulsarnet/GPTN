//
// Created by darkp on 15.10.2022.
//

#ifndef FFI_RUST_CONNECTCOMMAND_H
#define FFI_RUST_CONNECTCOMMAND_H

#include <QUndoCommand>

class GraphicScene;
class ArrowLine;

class ConnectCommand : public QUndoCommand {

public:

    enum ConnectionType {
        Connect,
        Disconnect,
        IncrementWeight,
        DicrementWeight,
        Bidirectional
    };

    explicit ConnectCommand(ArrowLine* connection,
                            ConnectionType type,
                            GraphicScene* scene,
                            QUndoCommand* parent = nullptr);

    void redo() override;
    void undo() override;

private:

    GraphicScene* m_scene;
    ArrowLine* m_connection;
    ConnectionType m_type;
};


#endif //FFI_RUST_CONNECTCOMMAND_H
