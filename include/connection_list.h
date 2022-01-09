//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_CONNECTION_LIST_H
#define FFI_RUST_CONNECTION_LIST_H

#include <QList>
#include "arrow_line.h"

class ConnectionList {


public:

    void add_line(ArrowLine* line) {
        connections.push_back(line);
    }

    void remove_line(ArrowLine* line) {
        auto it = std::find(connections.begin(), connections.end(), line);
        if (it != connections.end()) connections.erase(it);
    }

    void update_lines(PetriObject* object) {
        auto proceed = std::vector<ArrowLine*>();
        proceed.reserve(connections.size());

        for (auto connection: connections) {
            if (connection->from() == object || connection->to() == object) {

            }
        }
    }

private:

    QList<ArrowLine*> connections;
};

#endif //FFI_RUST_CONNECTION_LIST_H
