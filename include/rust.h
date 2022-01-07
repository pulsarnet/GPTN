//
// Created by nmuravev on 12/20/2021.
//

#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <iostream>
#include "petri_net.h"

extern "C" struct PetriNet;

extern "C" PetriNet* make();
extern "C" void del(PetriNet*);
extern "C" unsigned long count(PetriNet*);

extern "C" void add_position(PetriNet*, unsigned long);
extern "C" void add_transition(PetriNet*, unsigned long);
extern "C" void connect_p(PetriNet*, unsigned long, unsigned long);
extern "C" void connect_t(PetriNet*, unsigned long, unsigned long);

extern "C" struct FFIConnection {
    char* from;
    char* to;
};

extern "C" struct FFIBoxedSlice {
    char** elements;
    unsigned long len_elements;
    FFIConnection** connections;
    unsigned long len_connections;

    [[nodiscard]] InnerPetriNet into() const {
        QList<QString> r_elements;
        QList<std::tuple<QString, QString>> r_connections;

        auto cursor = this->elements;
        for (int i = 0; i < len_elements; i++) {
            r_elements.push_back(QString(*cursor));
            cursor++;
        }

        auto connections_cursor = this->connections;
        for (int i = 0; i < len_connections; i++) {
            auto s1 = QString((*connections_cursor)->from);
            auto s2 = QString((*connections_cursor)->to);

            r_connections.push_back(std::make_tuple(s1, s2));
            connections_cursor++;
        }

        return InnerPetriNet{ r_elements, r_connections };
    }
};

extern "C" struct SplitNet {
    FFIBoxedSlice** ptr;
    unsigned long size;
};

extern "C" FFIBoxedSlice* split(PetriNet*);

InnerPetriNet split_net(PetriNet* net) {
    auto result = split(net);
    return result->into();
}

#endif //FFI_RUST_RUST_H
