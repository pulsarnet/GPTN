//
// Created by nmuravev on 12/20/2021.
//

#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <iostream>

extern "C" struct PetriNet;

extern "C" PetriNet* make();
extern "C" void del(PetriNet*);
extern "C" unsigned long count(PetriNet*);

extern "C" void add_position(PetriNet*, unsigned long);
extern "C" void add_transition(PetriNet*, unsigned long);
extern "C" void connect_p(PetriNet*, unsigned long, unsigned long);
extern "C" void connect_t(PetriNet*, unsigned long, unsigned long);

extern "C" struct FFIBoxedSlice {
    char** ptr;
    char* partition_type;
    unsigned long size;
};

extern "C" struct SplitNet {
    FFIBoxedSlice** ptr;
    unsigned long size;

    [[nodiscard]] QList<QList<QString>> into_vec() const {
        QList<QList<QString>> result;

        auto cursor = this->ptr;
        for (int i = 0; i < size; i++) {
            result.emplace_back();

            auto str_cursor = (*cursor)->ptr;
            for (int j = 0; j < (*cursor)->size; j++) {
                result.back().emplace_back(QString(*str_cursor));
                str_cursor++;
            }
            cursor++;
        }

        return result;
    }
};

extern "C" SplitNet* split(PetriNet*);

QList<QList<QString>> split_net(PetriNet* net) {
    auto result = split(net);
    return result->into_vec();
}

#endif //FFI_RUST_RUST_H
