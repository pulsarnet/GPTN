//
// Created by nmuravev on 12/20/2021.
//

#ifndef FFI_RUST_RUST_H
#define FFI_RUST_RUST_H

#include <iostream>
#include "petri_net.h"
#include "matrix.h"
#include "named_matrix.h"

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

extern "C" struct FFIMatrix {
    unsigned long rows_len;
    unsigned long cols_len;
    long* matrix;

    [[nodiscard]] Matrix into() const {
        QList<long> elements;
        elements.resize(rows_len * cols_len, 0);

        for (int i = 0; i < rows_len; i++) {
            for (int j = 0; j < cols_len; j++) {
                elements[cols_len * i + j] = *(matrix + cols_len * i + j);
            }
        }

        return Matrix{rows_len, cols_len, elements};
    }
};

extern "C" struct FFINamedMatrix {
    char** rows;
    unsigned long rows_len;

    char** cols;
    unsigned long cols_len;
    long* matrix;

    [[nodiscard]] NamedMatrix into() const {
        QList<long> elements;
        elements.resize(rows_len * cols_len, 0);

        for (int i = 0; i < rows_len; i++) {
            for (int j = 0; j < cols_len; j++) {
                elements[cols_len * i + j] = *(matrix + cols_len * i + j);
            }
        }

        QList<QString> rows;
        QList<QString> cols;

        auto cursor = this->rows;
        for (int i = 0; i < rows_len; i++) {
            rows.push_back(QString(*cursor));
            cursor++;
        }

        cursor = this->cols;
        for (int i = 0; i < cols_len; i++) {
            cols.push_back(QString(*cursor));
            cursor++;
        }

        return NamedMatrix{rows, cols,elements};
    }
};

extern "C" struct CommonResult {
    FFIBoxedSlice* petri_net;
    FFIMatrix* c_matrix;
    FFINamedMatrix* d_matrix;
    FFINamedMatrix* lbf_matrix;
};

extern "C" CommonResult* split(PetriNet*);

InnerCommonResult split_net(PetriNet* net) {
    auto result = split(net);
    return InnerCommonResult { result->petri_net->into(), result->c_matrix->into(), result->d_matrix->into(), result->lbf_matrix->into() };
}

#endif //FFI_RUST_RUST_H
