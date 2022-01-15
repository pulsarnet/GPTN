//
// Created by Николай Муравьев on 09.01.2022.
//

#ifndef FFI_RUST_CONNECTION_LIST_H
#define FFI_RUST_CONNECTION_LIST_H

#include <QList>
#include "elements/arrow_line.h"
#include "petri_object.h"

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
        auto proceed_in = std::vector<ArrowLine*>();
        auto proceed_out = std::vector<ArrowLine*>();
        proceed_in.reserve(connections.size());
        proceed_out.reserve(connections.size());

        for (auto connection: connections) {
            if (connection->to() == object) proceed_in.push_back(connection);
            else if (connection->from() == object) proceed_out.push_back(connection);
        }

        for (auto connection_in : proceed_in) {
            for (auto connection_out : proceed_out) {

                if (connection_in->from() == connection_out->to()) {

                }

            }
        }
    }

private:

    QList<ArrowLine*> connections;
};

#endif //FFI_RUST_CONNECTION_LIST_H
