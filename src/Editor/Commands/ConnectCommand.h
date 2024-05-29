#ifndef FFI_RUST_CONNECTCOMMAND_H
#define FFI_RUST_CONNECTCOMMAND_H

#include <QUndoCommand>

class GraphicsScene;
class Edge;
class PetriObject;
class Position;
class Transition;

class ConnectCommand : public QUndoCommand {

    enum ConnectionType {
        Connect,
        Disconnect,
        SetWeight,
        Bidirectional
    };

    explicit ConnectCommand(Edge* connection,
                            ConnectionType type,
                            GraphicsScene* scene,
                            QUndoCommand* parent = nullptr);

    explicit ConnectCommand(Edge* connection,
                            ConnectionType type,
                            int newWeight,
                            int oldWeight,
                            bool reverse,
                            GraphicsScene* scene,
                            QUndoCommand* parent = nullptr);

public:

    static ConnectCommand* directed(PetriObject* from, PetriObject* to, GraphicsScene* scene);
    static ConnectCommand* inhibitor(Position *place, Transition *transition, GraphicsScene* scene);
    static ConnectCommand* disconnect(Edge* connection, GraphicsScene* scene);
    static ConnectCommand* setWeight(Edge* connection, int newWeight, bool reverse, GraphicsScene* scene);
    static ConnectCommand* setBidirectional(Edge* connection, GraphicsScene* scene);

    void redo() override;
    void undo() override;

private:

    GraphicsScene* m_scene;
    Edge* m_connection;
    ConnectionType m_type;
    int m_newWeight;
    int m_oldWeight;
    bool m_reverse;
};

#endif //FFI_RUST_CONNECTCOMMAND_H
